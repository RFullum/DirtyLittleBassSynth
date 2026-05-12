/*
  ==============================================================================

    PatchManager.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <optional>
#include <vector>

//==============================================================================

/// Owns the on-disk side of patch save/recall on top of the plugin's APVTS.
///
/// Patches are `.dlbs` files (XML inside) split into two sources:
///   - User patches:    OS-appropriate app-data directory, writable.
///   - Factory patches: shipped read-only in the plugin bundle's Resources/.
///                      Path is plumbed now; the bank is empty until the
///                      sound-design phase fills it.   // TODO: Factory Patches
/// All operations are message-thread only.
class PatchManager
    : private juce::AudioProcessorValueTreeState::Listener
{
public:
    enum class Source
    {
        User,
        Factory
    };

    enum class CurrentSource
    {
        Init,
        Factory,
        User
    };

    struct PatchInfo
    {
        juce::File   file;
        juce::String name;       // displayed name (file's stem, no extension)
        Source       source = Source::User;
    };

    explicit PatchManager(juce::AudioProcessorValueTreeState &apvtsToManage);
    ~PatchManager() override;

    void Init();

    juce::File GetUserPatchesDirectory()    const noexcept { return userPatchesDir; }
    juce::File GetFactoryPatchesDirectory() const noexcept { return factoryPatchesDir; }

    void RefreshPatchList();

    const std::vector<PatchInfo> &GetPatchList() const noexcept { return patchList; }

    juce::ValueTree                BuildPatchTree(const juce::String &patchName) const; // Excludes `tempo_fallback_bpm`
    std::optional<juce::ValueTree> ValidatePatchTree(const juce::ValueTree &patchRoot) const;

    void ApplyPatchTree(const juce::ValueTree &sanitizedRoot);
    void LoadInit();
    bool LoadPatch(const juce::File &file);
    bool SavePatch();
    bool SavePatchAs(const juce::String &requestedName);
    bool DeletePatch(const juce::File &file);

    std::optional<juce::File> ImportPatchFile(const juce::File &source);

    void SerializeToClipboard() const;
    bool ApplyFromClipboard();
    void StepPatch(int delta);  // Increment left or right through patches.
    void SetCurrentFromRestoredPath(const juce::File &path);
    void RandomizeAll();

    // TODO: Put behind JUCE_DEBUG deft?
    /// Generates `count` random patches named "Test Preset" (auto-incremented
    /// to "Test Preset 2", "Test Preset 3", …) under the user dir, then
    /// restores the APVTS state and current-patch tracking to whatever they
    /// were before the call. Used by the JUCE_DEBUG-only first-run seed in
    /// PluginProcessor so the patch system has something to navigate and
    /// browse against without manual patch creation.
    ///
    /// Safe to call at any time — the snapshot/restore makes it transparent.
    /// No-op when `count <= 0`.
    void GenerateTestPatches(int count = 5);

    juce::String  GetCurrentPatchName()     const noexcept { return currentPatchName; }
    juce::File    GetCurrentPatchFile()     const noexcept { return currentPatchFile; }
    CurrentSource GetCurrentSource()        const noexcept { return currentSource; }
    bool          IsCurrentPatchUserOwned() const noexcept { return currentSource == CurrentSource::User; }
    bool          IsCurrentPatchFactory()   const noexcept { return currentSource == CurrentSource::Factory; }

    static juce::String SanitizeFilename(const juce::String &requestedName);

    bool IsDirty() const noexcept { return isDirty.load(std::memory_order_acquire); }

private:
    void parameterChanged(const juce::String &paramID, float newValue) override;

    static constexpr const char *patchFileExtension = ".dlbs";
    static constexpr const char *patchRootTagName   = "DLBSPatch";

    static bool IsExcludedFromPatch(const juce::String &paramID) noexcept;
    static bool IsExcludedFromRandomize(const juce::String &paramID) noexcept;
    static bool IsReservedPatchName(const juce::String &name) noexcept;

    static juce::File ResolveUserPatchesDirectory();
    static juce::File ResolveFactoryPatchesDirectory();

    void       ScanDirectoryInto(const juce::File &dir, Source source, std::vector<PatchInfo> &out) const;
    juce::File MakeUniqueUserPatchFile(const juce::String &requestedName) const;
    void       SetCurrent(CurrentSource source, const juce::File &file, const juce::String &name);

    void RegisterParameterListeners();
    void UnregisterParameterListeners();
    
    void ClearDirty() noexcept { isDirty.store(false, std::memory_order_release); }

    juce::AudioProcessorValueTreeState &apvts;

    juce::File userPatchesDir;
    juce::File factoryPatchesDir;

    std::vector<PatchInfo> patchList;

    CurrentSource currentSource    { CurrentSource::Init };
    juce::File    currentPatchFile {};
    juce::String  currentPatchName { "Init" };

    /// Atomic because APVTS listener callbacks can fire from the audio thread
    /// when the host drives a parameter via automation.
    std::atomic<bool> isDirty { false };

    /// True during programmatic patch application (LoadInit / ApplyPatchTree).
    /// Each setValueNotifyingHost call we make would otherwise fire
    /// parameterChanged and falsely mark the patch dirty. The flag lets the
    /// listener short-circuit during loads while still catching genuine user
    /// or host edits afterward.
    std::atomic<bool> suppressDirty { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchManager)
};
