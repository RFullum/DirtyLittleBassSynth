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
    if (!userPatchesDir.exists())
        userPatchesDir.createDirectory();

    RefreshPatchList();
    RegisterParameterListeners();
}

void PatchManager::RefreshPatchList()
{
    std::vector<PatchInfo> factoryBlock;
    std::vector<PatchInfo> userBlock;

    ScanDirectoryInto(factoryPatchesDir, Source::Factory, factoryBlock);
    ScanDirectoryInto(userPatchesDir,    Source::User,    userBlock);

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
    // Hard reject: wrong root tag or no payload at all.
    if (!patchRoot.hasType(juce::Identifier(patchRootTagName)))
        return std::nullopt;

    if (patchRoot.getNumChildren() == 0)
        return std::nullopt;

    const auto sourceApvtsTree = patchRoot.getChild(0);

    if (sourceApvtsTree.getNumChildren() == 0)
        return std::nullopt;

    juce::ValueTree sanitizedRoot { patchRoot.getType() };
    sanitizedRoot.copyPropertiesFrom(patchRoot, nullptr);

    juce::ValueTree sanitizedApvts { sourceApvtsTree.getType() };

    for (int i = 0; i < sourceApvtsTree.getNumChildren(); ++i)
    {
        const auto srcChild = sourceApvtsTree.getChild(i);
        const auto id       = srcChild.getProperty("id").toString();

        if (IsExcludedFromPatch(id))
            continue;

        // Drop unknown paramIDs silently
        auto *param = apvts.getParameter(id);
        if (param == nullptr)
            continue;

        // Clamp raw value
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

void PatchManager::LoadInit()
{
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

bool PatchManager::SavePatch()
{
    // Init has no backing file and factory patches are read-only
    // Save acts as Save As for these.
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

bool PatchManager::DeletePatch(const juce::File &file)
{
    if (file.isAChildOf(factoryPatchesDir))
        return false;

    if (! file.existsAsFile())
        return false;

    if (! file.deleteFile())
        return false;

    if (file == currentPatchFile)
        LoadInit();

    RefreshPatchList();
    return true;
}

std::optional<juce::File> PatchManager::ImportPatchFile(const juce::File &source)
{
    if (! source.existsAsFile())
        return std::nullopt;

    const auto xml = juce::XmlDocument::parse(source);

    if (xml == nullptr)
        return std::nullopt;

    const auto root      = juce::ValueTree::fromXml(*xml);
    auto       sanitized = ValidatePatchTree(root);

    if (! sanitized.has_value())
        return std::nullopt;

    const auto destination = MakeUniqueUserPatchFile(source.getFileNameWithoutExtension());
    const auto finalName   = destination.getFileNameWithoutExtension();
    sanitized->setProperty("name", finalName, nullptr);

    const auto outXml = sanitized->createXml();

    if (outXml == nullptr)
        return std::nullopt;

    if (! outXml->writeTo(destination))
        return std::nullopt;

    return destination;
}

void PatchManager::SerializeToClipboard() const
{
    const auto tree = BuildPatchTree(currentPatchName);
    const auto xml  = tree.createXml();

    if (xml == nullptr)
        return;

    juce::SystemClipboard::copyTextToClipboard(xml->toString());
}

bool PatchManager::ApplyFromClipboard()
{
    const auto clipboardText = juce::SystemClipboard::getTextFromClipboard();

    if (clipboardText.isEmpty())
        return false;

    const auto xml = juce::parseXML(clipboardText);

    if (xml == nullptr)
        return false;

    const auto root      = juce::ValueTree::fromXml(*xml);
    auto       sanitized = ValidatePatchTree(root);

    if (! sanitized.has_value())
        return false;

    ApplyPatchTree(*sanitized);

    // ApplyPatchTree suppresses the listener during its writes
    isDirty.store(true, std::memory_order_release);

    return true;
}

void PatchManager::StepPatch(int delta)
{
    if (patchList.empty() || delta == 0)
        return;

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

    // Wrapping
    if (currentIndex < 0)
        nextIndex = (delta > 0) ? 0 : listSize - 1;
    else
        nextIndex = ((currentIndex + delta) % listSize + listSize) % listSize;

    LoadPatch(patchList[(size_t) nextIndex].file);
}

void PatchManager::SetCurrentFromRestoredPath(const juce::File &path)
{
    if (path == juce::File{} || ! path.existsAsFile())
        return;

    const auto source = path.isAChildOf(factoryPatchesDir)
                            ? CurrentSource::Factory
                            : CurrentSource::User;

    juce::String name;
    if (auto xml = juce::XmlDocument::parse(path))
    {
        const auto root = juce::ValueTree::fromXml(*xml);
        name = root.getProperty("name").toString();
    }
    if (name.isEmpty())
        name = path.getFileNameWithoutExtension();

    SetCurrent(source, path, name);
    ClearDirty();
}

void PatchManager::RandomizeAll()
{
    auto       &rng   = juce::Random::getSystemRandom();
    const auto  state = apvts.copyState();

    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto id = state.getChild(i).getProperty("id").toString();

        if (IsExcludedFromRandomize(id))
            continue;

        if (auto *param = apvts.getParameter(id))
            param->setValueNotifyingHost(rng.nextFloat());
    }

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
}

juce::String PatchManager::SanitizeFilename(const juce::String &requestedName)
{
    auto sanitized = juce::File::createLegalFileName(requestedName).trim();
    if (sanitized.isEmpty())
        sanitized = "Untitled";

    return sanitized;
}

void PatchManager::parameterChanged(const juce::String & /*paramID*/, float /*newValue*/)
{
    if (suppressDirty.load(std::memory_order_acquire))
        return;

    isDirty.store(true, std::memory_order_release);
}

bool PatchManager::IsExcludedFromPatch(const juce::String &paramID) noexcept
{
    return paramID == "tempo_fallback_bpm";
}

bool PatchManager::IsExcludedFromRandomize(const juce::String &paramID) noexcept
{
    // Patch exclusions are also randomise exclusions
    if (IsExcludedFromPatch(paramID))    return true;
    if (paramID == "pitch_bend_range")   return true;
    if (paramID == "master_gain")        return true;
    if (paramID == "master_wide")        return true;
    if (paramID == "mono_below_freq")    return true;
    if (paramID == "limiter_on")         return true;
    if (paramID == "limiter_ceiling")    return true;

    return false;
}

bool PatchManager::IsReservedPatchName(const juce::String &name) noexcept
{
    // "Init" is the label for the no-patch-loaded state and the INIT button.
    // Allowing a user patch with this name would cause the popup, title-header
    // colouring, and INIT button to disagree about what "Init" means.
    return name.equalsIgnoreCase("Init");
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
    // alongside the binary on other platforms. RefreshPatchList tolerates a
    // missing factory dir.
    auto exe = juce::File::getSpecialLocation(juce::File::currentApplicationFile);

   #if JUCE_MAC
    return exe.getChildFile("Contents/Resources/Patches");
   #else
    return exe.getParentDirectory().getChildFile("Patches");
   #endif
}

void PatchManager::ScanDirectoryInto(const juce::File &dir, Source source, std::vector<PatchInfo> &out) const
{
    if (! dir.isDirectory())
        return;

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

juce::File PatchManager::MakeUniqueUserPatchFile(const juce::String &requestedName) const
{
    const auto baseName = SanitizeFilename(requestedName);
    const auto nameClashes = [this] (const juce::String &candidate)
    {
        if (IsReservedPatchName(candidate))                                                return true;
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

void PatchManager::SetCurrent(CurrentSource source, const juce::File &file, const juce::String &name)
{
    currentSource    = source;
    currentPatchFile = file;
    currentPatchName = name;
}

void PatchManager::RegisterParameterListeners()
{
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
