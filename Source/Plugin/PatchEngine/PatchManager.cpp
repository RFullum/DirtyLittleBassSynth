/*
  ==============================================================================

    PatchManager.cpp

  ==============================================================================
*/

#include "PatchManager.h"
#include <algorithm>

//==============================================================================

PatchManager::PatchManager(juce::AudioProcessorValueTreeState &apvtsToManage)
: apvts(apvtsToManage)
, userPatchesDir   (ResolveUserPatchesDirectory())
, factoryPatchesDir(ResolveFactoryPatchesDirectory())
{}

PatchManager::~PatchManager()
{
    UnregisterParameterListeners();
}

void PatchManager::Init()
{
    // Create the user patches directory on first run. createDirectory walks the
    // full path, so missing parents (FullumMusic/, Dirty Little Bass Synth/)
    // are created in a single call. Result is ignored intentionally — if
    // creation fails (read-only volume, permissions), RefreshPatchList just
    // returns an empty user-side list and the UI degrades gracefully.
    if (! userPatchesDir.exists())
        userPatchesDir.createDirectory();

    RefreshPatchList();
    RegisterParameterListeners();
}


void PatchManager::RefreshPatchList()
{
    // Scan each source into its own block so we can sort the two halves
    // independently before concatenating. Factory always comes first in the
    // final order regardless of how they sort against user names.
    std::vector<PatchInfo> factoryBlock;
    std::vector<PatchInfo> userBlock;

    ScanDirectoryInto(factoryPatchesDir, Source::Factory, factoryBlock);
    ScanDirectoryInto(userPatchesDir,    Source::User,    userBlock);

    // compareNatural gives us "0-9 then A-Z" with sane number ordering inside
    // names ("Patch 2" before "Patch 10"). Stable across macOS / Win / Linux,
    // unlike findChildFiles which is OS-dependent.
    const auto byNameNatural = [] (const PatchInfo &a, const PatchInfo &b)
    {
        return a.name.compareNatural(b.name) < 0;
    };

    std::sort(factoryBlock.begin(), factoryBlock.end(), byNameNatural);
    std::sort(userBlock   .begin(), userBlock   .end(), byNameNatural);

    std::vector<PatchInfo> next;
    next.reserve(factoryBlock.size() + userBlock.size());
    next.insert(next.end(), factoryBlock.begin(), factoryBlock.end());
    next.insert(next.end(), userBlock   .begin(), userBlock   .end());

    patchList = std::move(next);
}

void PatchManager::ScanDirectoryInto(const juce::File &dir, Source source, std::vector<PatchInfo> &out) const
{
    if (! dir.isDirectory())
        return;

    // findChildFiles is a one-shot enumeration; cheap enough at the patch-list
    // sizes we expect (dozens, not thousands).
    auto files = dir.findChildFiles(juce::File::findFiles
                                    , false     // searchRecursively
                                    , juce::String("*") + patchFileExtension);

    for (const auto &f : files)
    {
        PatchInfo info;
        info.file   = f;
        info.name   = f.getFileNameWithoutExtension();
        info.source = source;
        out.push_back(std::move(info));
    }
}


juce::File PatchManager::ResolveUserPatchesDirectory()
{
    // ~/Library/Application Support/FullumMusic/Dirty Little Bass Synth/Patches/
    // (and the OS-equivalents on Windows/Linux). One folder per company so all
    // FullumMusic plugins can share neighbours, then per-plugin so they don't
    // step on each other.
    auto base = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

   #if JUCE_MAC
    base = base.getChildFile("Application Support");
   #endif

    return base.getChildFile("FullumMusic")
               .getChildFile("Dirty Little Bass Synth")
               .getChildFile("Patches");
}


juce::File PatchManager::ResolveFactoryPatchesDirectory()
{
    // Factory patches live inside the plugin bundle's Resources/ on macOS, or
    // alongside the binary on other platforms. Path is computed eagerly even
    // though the directory may not exist yet — RefreshPatchList tolerates a
    // missing factory dir.
    auto exe = juce::File::getSpecialLocation(juce::File::currentApplicationFile);

   #if JUCE_MAC
    return exe.getChildFile("Contents/Resources/Patches");
   #else
    return exe.getParentDirectory().getChildFile("Patches");
   #endif
}

bool PatchManager::IsExcludedFromPatch(const juce::String &paramID) noexcept
{
    // Keep this list intentionally tiny — anything sound-design-related rides
    // with the patch (including pitch_bend_range, by explicit user decision).
    return paramID == "tempo_fallback_bpm";
}

bool PatchManager::IsExcludedFromRandomize(const juce::String &paramID) noexcept
{
    // Patch exclusions are also randomise exclusions (no point randomising
    // something that doesn't live in patches).
    if (IsExcludedFromPatch(paramID))    return true;

    // Pitch bend range is a controller-config preference per the user spec —
    // same reasoning as why patches don't reset it.
    if (paramID == "pitch_bend_range")   return true;

    // Master-section safety params are written to known-safe values by
    // RandomizeAll itself; randomising them first would just be wasted work.
    if (paramID == "master_gain")        return true;
    if (paramID == "master_wide")        return true;
    if (paramID == "mono_below_freq")    return true;
    if (paramID == "limiter_on")         return true;
    if (paramID == "limiter_ceiling")    return true;

    return false;
}

juce::ValueTree PatchManager::BuildPatchTree(const juce::String &patchName) const
{
    // Deep copy so pruning excluded params doesn't disturb the live state.
    auto state = apvts.copyState();

    for (int i = state.getNumChildren() - 1; i >= 0; --i)
    {
        const auto child = state.getChild(i);
        const auto id    = child.getProperty("id").toString();

        if (IsExcludedFromPatch(id))
            state.removeChild(i, nullptr);
    }

    juce::ValueTree root { juce::Identifier(patchRootTagName) };
    root.setProperty("name",          patchName,                nullptr);
    root.setProperty("pluginVersion", JucePlugin_VersionString, nullptr);
    root.appendChild(state, nullptr);

    return root;
}

std::optional<juce::ValueTree> PatchManager::ValidatePatchTree(const juce::ValueTree &patchRoot) const
{
    // Hard reject: wrong root tag or no payload at all. Everything else is
    // recoverable by dropping the bad bits.
    if (! patchRoot.hasType(juce::Identifier(patchRootTagName)))
        return std::nullopt;

    if (patchRoot.getNumChildren() == 0)
        return std::nullopt;

    // APVTS state is the first (and only) child. We grab it by index rather
    // than by type-id so we don't hardcode the APVTS state name
    // ("ParameterTree" — set in PluginProcessor's parameters initializer).
    const auto sourceApvtsTree = patchRoot.getChild(0);

    if (sourceApvtsTree.getNumChildren() == 0)
        return std::nullopt;

    // Build a sanitized copy from scratch rather than mutating the input.
    // Callers (drag-drop, paste) may want to inspect the original if we
    // return nullopt for some other reason later.
    juce::ValueTree sanitizedRoot { patchRoot.getType() };
    sanitizedRoot.copyPropertiesFrom(patchRoot, nullptr);

    juce::ValueTree sanitizedApvts { sourceApvtsTree.getType() };

    for (int i = 0; i < sourceApvtsTree.getNumChildren(); ++i)
    {
        const auto srcChild = sourceApvtsTree.getChild(i);
        const auto id       = srcChild.getProperty("id").toString();

        // Drop excluded params (tempo_fallback_bpm rides per-session).
        if (IsExcludedFromPatch(id))
            continue;

        // Drop unknown paramIDs silently — forward compat for patches saved
        // before a param was added or after one was renamed.
        auto *param = apvts.getParameter(id);
        if (param == nullptr)
            continue;

        // Clamp raw value to declared range. Out-of-range values can arrive
        // from corrupted files, hand-edited XML, or stale patches that
        // pre-date a tightened range.
        const auto  range    = param->getNormalisableRange();
        const float rawValue = (float) srcChild.getProperty("value");
        const float clamped  = juce::jlimit(range.start, range.end, rawValue);

        juce::ValueTree paramCopy { srcChild.getType() };
        paramCopy.copyPropertiesFrom(srcChild, nullptr);
        paramCopy.setProperty("value", clamped, nullptr);

        sanitizedApvts.appendChild(paramCopy, nullptr);
    }

    // A patch with zero usable params after filtering isn't a patch — reject.
    if (sanitizedApvts.getNumChildren() == 0)
        return std::nullopt;

    sanitizedRoot.appendChild(sanitizedApvts, nullptr);
    return sanitizedRoot;
}

void PatchManager::ApplyPatchTree(const juce::ValueTree &sanitizedRoot)
{
    // Programmatic param storm — gate the dirty flag so parameterChanged
    // ignores our writes. Callers (LoadPatch) are responsible for the final
    // ClearDirty() once the apply completes.
    suppressDirty.store(true, std::memory_order_release);

    const auto apvtsTree = sanitizedRoot.getChild(0);

    for (int i = 0; i < apvtsTree.getNumChildren(); ++i)
    {
        const auto child = apvtsTree.getChild(i);
        const auto id    = child.getProperty("id").toString();

        if (auto *param = apvts.getParameter(id))
        {
            // PARAM children store the non-normalised raw value. Convert to
            // [0, 1] for setValueNotifyingHost.
            const float rawValue = (float) child.getProperty("value");
            param->setValueNotifyingHost(param->convertTo0to1(rawValue));
        }
    }

    suppressDirty.store(false, std::memory_order_release);
}

//==============================================================================

void PatchManager::LoadInit()
{
    // Reset every non-excluded param to its declared normalised default.
    // Iterating the live state tree gives us the IDs as registered with APVTS;
    // setValueNotifyingHost ensures attached UI controls and the host's
    // automation lane see the change (replaceState would bypass listeners).
    suppressDirty.store(true, std::memory_order_release);

    auto state = apvts.copyState();

    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto child = state.getChild(i);
        const auto id    = child.getProperty("id").toString();

        if (IsExcludedFromPatch(id))
            continue;

        if (auto *param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->getDefaultValue());
    }

    SetCurrent(CurrentSource::Init, juce::File{}, "Init");

    suppressDirty.store(false, std::memory_order_release);
    ClearDirty();
}

//==============================================================================

bool PatchManager::LoadPatch(const juce::File &file)
{
    if (! file.existsAsFile())
        return false;

    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr)
        return false;

    const auto root      = juce::ValueTree::fromXml(*xml);
    auto       sanitized = ValidatePatchTree(root);

    if (! sanitized.has_value())
        return false;

    ApplyPatchTree(*sanitized);

    // Determine source by location. Files outside both managed directories
    // (e.g. a one-off shared patch sitting on the Desktop) are treated as
    // User loads — they'll be copied into the user dir later when we wire
    // drag-and-drop import.
    const auto source = file.isAChildOf(factoryPatchesDir)
                            ? CurrentSource::Factory
                            : CurrentSource::User;

    // Prefer the patch's stored name; fall back to the filename stem if the
    // file pre-dates name metadata or has an empty `name` property.
    auto name = root.getProperty("name").toString();
    if (name.isEmpty())
        name = file.getFileNameWithoutExtension();

    SetCurrent(source, file, name);
    ClearDirty();
    return true;
}

//==============================================================================

bool PatchManager::SavePatch()
{
    // Init has no backing file and factory patches are read-only by contract.
    // GUI is expected to open the Save As dialog when this returns false.
    if (currentSource != CurrentSource::User)
        return false;

    if (! currentPatchFile.getParentDirectory().isDirectory())
        return false;

    const auto tree = BuildPatchTree(currentPatchName);
    const auto xml  = tree.createXml();

    if (xml == nullptr)
        return false;

    if (! xml->writeTo(currentPatchFile))
        return false;

    RefreshPatchList();
    ClearDirty();
    return true;
}

//==============================================================================

bool PatchManager::SavePatchAs(const juce::String &requestedName)
{
    const auto destination = MakeUniqueUserPatchFile(requestedName);
    const auto finalName   = destination.getFileNameWithoutExtension();

    const auto tree = BuildPatchTree(finalName);
    const auto xml  = tree.createXml();

    if (xml == nullptr)
        return false;

    if (! xml->writeTo(destination))
        return false;

    SetCurrent(CurrentSource::User, destination, finalName);
    RefreshPatchList();
    ClearDirty();
    return true;
}

//==============================================================================

bool PatchManager::DeletePatch(const juce::File &file)
{
    // Factory patches are read-only by contract. Even if they're sitting on a
    // writable volume (e.g. during dev), the manager refuses to touch them.
    if (file.isAChildOf(factoryPatchesDir))
        return false;

    if (! file.existsAsFile())
        return false;

    if (! file.deleteFile())
        return false;

    // Per the UX spec: deleting the currently-loaded patch drops to Init so
    // the user is never left looking at a ghost name. Live params reset to
    // defaults as part of LoadInit().
    if (file == currentPatchFile)
        LoadInit();

    RefreshPatchList();
    return true;
}

//==============================================================================

void PatchManager::StepPatch(int delta)
{
    // Empty list: nowhere to go.
    if (patchList.empty() || delta == 0)
        return;

    // Locate the current file in the list. Init's empty File never matches a
    // real patch entry, so coming from Init falls through to the "not found"
    // branch below — exactly what we want (Init isn't a list member).
    int currentIndex = -1;

    for (size_t i = 0; i < patchList.size(); ++i)
    {
        if (patchList[i].file == currentPatchFile)
        {
            currentIndex = (int) i;
            break;
        }
    }

    const int listSize = (int) patchList.size();
    int       nextIndex;

    if (currentIndex < 0)
    {
        // No anchor — treat the step as the user's first navigation:
        // forward → first patch, backward → last patch.
        nextIndex = (delta > 0) ? 0 : listSize - 1;
    }
    else
    {
        // Wrap modulo listSize, using the positive-modulo idiom so negative
        // deltas don't yield a negative index.
        nextIndex = ((currentIndex + delta) % listSize + listSize) % listSize;
    }

    LoadPatch(patchList[(size_t) nextIndex].file);
}

//==============================================================================

void PatchManager::RandomizeAll()
{
    auto       &rng   = juce::Random::getSystemRandom();
    const auto  state = apvts.copyState();

    // 1. Random-roll every non-excluded param to a fresh [0, 1] value.
    //    Listener fires on each call (suppressDirty stays false) so the patch
    //    correctly ends up dirty after the storm.
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto id = state.getChild(i).getProperty("id").toString();

        if (IsExcludedFromRandomize(id))
            continue;

        if (auto *param = apvts.getParameter(id))
            param->setValueNotifyingHost(rng.nextFloat());
    }

    // 2. Force the master-section safety params to known-safe raw values.
    //    Done after the random pass so we don't waste cycles randomising
    //    something that's about to be overwritten anyway, and so the order
    //    matches the user's mental model ("randomise, then make it safe").
    const auto setRaw = [this] (const char *id, float raw)
    {
        if (auto *param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(raw));
    };

    setRaw("master_gain",     1.0f);     // unity (0 dB)
    setRaw("master_wide",     0.0f);     // centred — no widening, plays as mono
    setRaw("mono_below_freq", 120.0f);   // default crossover
    setRaw("limiter_on",      0.0f);     // off
    setRaw("limiter_ceiling", -0.1f);    // default ceiling (harmless while limiter off)

    // No SetCurrent, no ClearDirty: randomise leaves the user on the same
    // patch name with unsaved changes (every setValueNotifyingHost above
    // flipped isDirty via the listener).
}

//==============================================================================

void PatchManager::GenerateTestPatches(int count)
{
    if (count <= 0)
        return;

    // Snapshot what's loaded right now so we can put everything back when
    // we're done. BuildPatchTree wraps current state minus excluded params
    // (tempo_fallback_bpm), which is fine — RandomizeAll doesn't touch those
    // anyway, so no restore is needed for them.
    const auto stateSnapshot = BuildPatchTree("__snapshot__");

    const auto originalSource = currentSource;
    const auto originalFile   = currentPatchFile;
    const auto originalName   = currentPatchName;
    const bool originalDirty  = isDirty.load(std::memory_order_acquire);

    // Generate the patches. Each iteration randomises and saves; SavePatchAs
    // auto-increments "Test Preset" → "Test Preset 2" → … so collisions with
    // existing files (e.g. on second debug run) are handled transparently.
    for (int i = 0; i < count; ++i)
    {
        RandomizeAll();
        SavePatchAs("Test Preset");
    }

    // Restore APVTS state. ValidatePatchTree round-trips the snapshot through
    // the same sanitiser the public load path uses; ApplyPatchTree drives
    // setValueNotifyingHost with suppressDirty engaged so the listener
    // doesn't mark the patch dirty during restore.
    if (auto sanitized = ValidatePatchTree(stateSnapshot))
        ApplyPatchTree(*sanitized);

    // Restore current-patch tracking. The save loop above moved currentSource
    // to User and currentPatchFile to the last saved file; this puts it back.
    SetCurrent(originalSource, originalFile, originalName);

    // Restore dirty exactly as it was — the loop's SavePatchAs calls cleared
    // it, so we have to explicitly re-set if the caller was dirty.
    if (originalDirty)
        isDirty.store(true, std::memory_order_release);
    else
        ClearDirty();
}

//==============================================================================

juce::String PatchManager::SanitizeFilename(const juce::String &requestedName)
{
    // createLegalFileName strips any character the host OS would reject for a
    // filename. We trim afterward so leading/trailing whitespace doesn't
    // become part of the on-disk name (looks identical in UI, confuses Finder).
    auto sanitized = juce::File::createLegalFileName(requestedName).trim();

    // Empty input (or input made entirely of stripped chars) falls back to
    // "Untitled" so SavePatchAs always has something writeable. The
    // auto-increment in MakeUniqueUserPatchFile will turn this into
    // "Untitled 2", "Untitled 3", … as needed.
    if (sanitized.isEmpty())
        sanitized = "Untitled";

    return sanitized;
}

juce::File PatchManager::MakeUniqueUserPatchFile(const juce::String &requestedName) const
{
    const auto baseName = SanitizeFilename(requestedName);

    // Predicate: does any patch already exist with this name, in either dir?
    // Checking both factory and user dirs prevents the popup from later
    // showing two entries with the same display name.
    const auto nameClashes = [this] (const juce::String &candidate)
    {
        if (userPatchesDir   .getChildFile(candidate + patchFileExtension).existsAsFile()) return true;
        if (factoryPatchesDir.getChildFile(candidate + patchFileExtension).existsAsFile()) return true;
        return false;
    };

    auto candidate = baseName;
    int  suffix    = 2;

    while (nameClashes(candidate))
        candidate = baseName + " " + juce::String(suffix++);

    return userPatchesDir.getChildFile(candidate + patchFileExtension);
}

//==============================================================================

void PatchManager::SetCurrent(CurrentSource source, const juce::File &file, const juce::String &name)
{
    currentSource    = source;
    currentPatchFile = file;
    currentPatchName = name;
}

//==============================================================================

void PatchManager::RegisterParameterListeners()
{
    // Walk the APVTS state to enumerate every registered paramID and subscribe
    // to all the non-excluded ones. APVTS holds listeners by raw pointer keyed
    // on paramID, so re-registering for the same (id, listener) is a no-op
    // risk we avoid by only calling this once from Init().
    const auto state = apvts.copyState();

    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto id = state.getChild(i).getProperty("id").toString();

        if (IsExcludedFromPatch(id))
            continue;

        apvts.addParameterListener(id, this);
    }
}

void PatchManager::UnregisterParameterListeners()
{
    const auto state = apvts.copyState();

    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto id = state.getChild(i).getProperty("id").toString();

        if (IsExcludedFromPatch(id))
            continue;

        apvts.removeParameterListener(id, this);
    }
}

//==============================================================================

void PatchManager::parameterChanged(const juce::String & /*paramID*/, float /*newValue*/)
{
    // Programmatic load paths set suppressDirty so this callback ignores their
    // setValueNotifyingHost storm. Genuine user / host edits land here with
    // the flag cleared and flip dirty true.
    if (suppressDirty.load(std::memory_order_acquire))
        return;

    isDirty.store(true, std::memory_order_release);
}
