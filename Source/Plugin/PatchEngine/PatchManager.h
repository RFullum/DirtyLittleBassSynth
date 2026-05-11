/*
  ==============================================================================

    PatchManager.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <optional>
#include <vector>

//==============================================================================

/// Owns the on-disk side of patch save/recall on top of the plugin's APVTS.
///
/// Patches are `.dlbs` files (XML inside) split into two sources:
///   - User patches:    OS-appropriate app-data directory, writable.
///   - Factory patches: shipped read-only in the plugin bundle's Resources/.
///                      Path is plumbed now; the bank is empty until the
///                      sound-design phase fills it.
///
/// This is the skeleton stage — directory bootstrap and the patch-list cache
/// only. Save/Load, dirty tracking, and the various current-patch state flags
/// are added in follow-up steps as the GUI components are wired up.
///
/// All operations are message-thread only.
class PatchManager
{
public:
    /// Distinguishes a user patch (writable, deletable) from a factory one
    /// (read-only) so the UI can disable destructive operations on factory
    /// items.
    enum class Source
    {
        User,
        Factory
    };

    struct PatchInfo
    {
        juce::File   file;
        juce::String name;       // displayed name (file's stem, no extension)
        Source       source = Source::User;
    };

    explicit PatchManager(juce::AudioProcessorValueTreeState &apvtsToManage);
    ~PatchManager() = default;

    //==========================================================================
    // Setup.
    //==========================================================================

    /// Creates the user patches directory if absent and populates the patch
    /// list cache. Safe to call once from the processor's constructor — does
    /// no audio-thread work and no APVTS mutation.
    void Init();

    //==========================================================================
    // Directory accessors.
    //==========================================================================

    /// Per-user, OS-appropriate patches folder. Created on Init().
    juce::File GetUserPatchesDirectory()    const noexcept { return userPatchesDir; }

    /// Bundle-relative read-only factory patches folder. May not exist yet
    /// (populated in the final sound-design phase).
    juce::File GetFactoryPatchesDirectory() const noexcept { return factoryPatchesDir; }

    //==========================================================================
    // Patch list (cached; refresh on demand).
    //==========================================================================

    /// Rescans both directories for *.dlbs files and rebuilds the cached list.
    /// Factory entries land first, then user entries; each group is sorted by
    /// filename in the order the OS returns them (alphabetical on macOS).
    void RefreshPatchList();

    /// Cached snapshot of all known patches. Populated by RefreshPatchList().
    const std::vector<PatchInfo> &GetPatchList() const noexcept { return patchList; }

    //==========================================================================
    // Patch tree construction / validation / application.
    //==========================================================================

    /// Builds a `<DLBSPatch>` ValueTree wrapping a filtered copy of the current
    /// APVTS state, tagged with the supplied patch name and the running plugin
    /// version. Used by every save path. Excludes `tempo_fallback_bpm` (a
    /// per-session setup param that shouldn't ride with the sound design).
    juce::ValueTree BuildPatchTree(const juce::String &patchName) const;

    /// Validates and sanitizes a `<DLBSPatch>` ValueTree from any source
    /// (file load, clipboard paste, dropped file). Returns:
    ///   - std::nullopt on hard failure: missing root tag or no APVTS payload.
    ///   - A sanitized copy on success: unknown paramIDs are dropped silently
    ///     (forward compat for older patches saved before a param existed),
    ///     out-of-range values are clamped to the declared range, and the
    ///     excluded paramID set is stripped.
    /// The returned tree is safe to pass directly to ApplyPatchTree.
    std::optional<juce::ValueTree> ValidatePatchTree(const juce::ValueTree &patchRoot) const;

    /// Applies a sanitized DLBSPatch ValueTree to the live APVTS via
    /// setValueNotifyingHost so UI attachments and the host's automation lane
    /// see every change. Caller must run ValidatePatchTree first; this method
    /// trusts that the input is already filtered and clamped.
    void ApplyPatchTree(const juce::ValueTree &sanitizedRoot);

private:
    static constexpr const char *patchFileExtension = ".dlbs";
    static constexpr const char *patchRootTagName   = "DLBSPatch";

    /// True for paramIDs excluded from patch save / load. Currently just
    /// `tempo_fallback_bpm` — a per-session setup param.
    static bool IsExcludedFromPatch(const juce::String &paramID) noexcept;

    /// Resolves the user patches directory path. Pure path computation, no I/O.
    static juce::File ResolveUserPatchesDirectory();

    /// Resolves the factory patches directory inside the plugin bundle's
    /// Resources/. Pure path computation.
    static juce::File ResolveFactoryPatchesDirectory();

    /// Scans `dir` for *.dlbs files, appending each as a PatchInfo with the
    /// given source. No-op if `dir` doesn't exist.
    void ScanDirectoryInto(const juce::File &dir, Source source, std::vector<PatchInfo> &out) const;

    juce::AudioProcessorValueTreeState &apvts;

    juce::File userPatchesDir;
    juce::File factoryPatchesDir;

    std::vector<PatchInfo> patchList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchManager)
};
