/*
  ==============================================================================

    PatchManager.cpp

  ==============================================================================
*/

#include "PatchManager.h"

//==============================================================================

PatchManager::PatchManager(juce::AudioProcessorValueTreeState &apvtsToManage)
: apvts(apvtsToManage)
, userPatchesDir   (ResolveUserPatchesDirectory())
, factoryPatchesDir(ResolveFactoryPatchesDirectory())
{}

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
}


void PatchManager::RefreshPatchList()
{
    std::vector<PatchInfo> next;

    // Factory first so they sort to the top of the menu when the UI iterates
    // the list in order.
    ScanDirectoryInto(factoryPatchesDir, Source::Factory, next);
    ScanDirectoryInto(userPatchesDir,    Source::User,    next);

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
    // Dirty-flag suppression around this driver lands in Phase A4. For now
    // every setValueNotifyingHost call below would mark the patch dirty, but
    // there's no listener yet so the side effect is harmless.
    const auto apvtsTree = sanitizedRoot.getChild(0);

    for (int i = 0; i < apvtsTree.getNumChildren(); ++i)
    {
        const auto child = apvtsTree.getChild(i);
        const auto id    = child.getProperty("id").toString();

        if (auto *param = apvts.getParameter(id))
        {
            // PARAM children store the non-normalised raw value. Convert to
            // [0, 1] for setValueNotifyingHost, which then notifies attached
            // UI controls and the host's automation lane.
            const float rawValue = (float) child.getProperty("value");
            param->setValueNotifyingHost(param->convertTo0to1(rawValue));
        }
    }
}
