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
///                      sound-design phase fills it.
///
/// This is the skeleton stage — directory bootstrap and the patch-list cache
/// only. Save/Load, dirty tracking, and the various current-patch state flags
/// are added in follow-up steps as the GUI components are wired up.
///
/// All operations are message-thread only.
class PatchManager  : private juce::AudioProcessorValueTreeState::Listener
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

    /// State of the currently-loaded patch. Init = no file backing the current
    /// param state (defaults or freshly randomised). Factory/User = loaded
    /// from disk. Drives the UI's "Save behaves as Save As" rule and the
    /// Delete-button enable gate.
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

    //==========================================================================
    // Setup.
    //==========================================================================

    /// Creates the user patches directory if absent, populates the patch list
    /// cache, and registers parameter listeners for dirty tracking. Safe to
    /// call once from the processor's constructor — does no audio-thread work
    /// and no APVTS mutation.
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

    //==========================================================================
    // Save / Load / Delete.
    //==========================================================================

    /// Resets every non-excluded APVTS param to its declared default value and
    /// marks the current state as Init (no backing file). `tempo_fallback_bpm`
    /// is preserved (it's a per-session setup param, not sound design).
    void LoadInit();

    /// Reads a .dlbs file, validates it via ValidatePatchTree, and applies the
    /// result. Returns false if the file is missing, malformed, or has no
    /// usable payload — in that case the live state is left untouched. On
    /// success, updates current-patch tracking based on which directory the
    /// file lives in.
    bool LoadPatch(const juce::File &file);

    /// Overwrites the currently-loaded user patch on disk with the live APVTS
    /// state. Returns false when the current state isn't user-owned (Init or
    /// Factory) or the write fails — the GUI is expected to open the Save As
    /// dialog when this returns false.
    bool SavePatch();

    /// Writes the live APVTS state to a new .dlbs file under the user dir
    /// using `requestedName` as the starting point. Filesystem-illegal chars
    /// are stripped, and the name is auto-incremented (" 2", " 3", …) if a
    /// patch with that name already exists in either the user or factory
    /// directory. Updates current-patch tracking on success.
    /// Returns false if writing fails.
    bool SavePatchAs(const juce::String &requestedName);

    /// Deletes a user patch from disk. Refuses to delete factory patches
    /// (returns false). If the deleted file was the currently-loaded patch,
    /// drops to Init so the UI doesn't keep showing a ghost. Refreshes the
    /// patch list on success.
    /// Returns false if the file is factory-owned, missing, or deletion fails.
    bool DeletePatch(const juce::File &file);

    //==========================================================================
    // Navigation.
    //==========================================================================

    /// Steps to the previous (-1) or next (+1) patch in the sorted list,
    /// wrapping at the ends. Used by the title-header prev/next arrows.
    /// Behavior:
    ///   - Empty patch list: no-op.
    ///   - Current is in the list: step by delta, wrap modulo list size.
    ///   - Current is Init (or a deleted-file ghost): delta > 0 lands on the
    ///     first patch, delta < 0 lands on the last. Init isn't a member of
    ///     the list so it never appears mid-cycle.
    void StepPatch(int delta);

    //==========================================================================
    // Randomize.
    //==========================================================================

    /// Rolls every non-excluded, non-safety param to a random normalised value
    /// and forces the five master-section "safety" params to known-safe
    /// values (unity output gain, no widening, mono crossover at default,
    /// limiter off, ceiling default). This prevents the first randomise from
    /// destroying the user's ears or pinning the limiter.
    ///
    /// Excluded entirely (untouched by randomise):
    ///   - tempo_fallback_bpm  (per-session setup)
    ///   - pitch_bend_range    (controller config, not sound design)
    ///
    /// Forced to safety values:
    ///   - master_gain, master_wide, mono_below_freq,
    ///     limiter_on, limiter_ceiling
    ///
    /// Doesn't change patch name or source — randomise from any patch leaves
    /// the user on that patch with unsaved changes (dirty `*`).
    void RandomizeAll();

    //==========================================================================
    // Dev / testing utilities.
    //==========================================================================

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

    //==========================================================================
    // Current-patch state (read-only accessors).
    //==========================================================================

    /// Display name of the currently-loaded patch. "Init" when no patch is
    /// loaded; otherwise the name stored in the .dlbs file (or the filename
    /// stem if the file pre-dates name metadata).
    juce::String GetCurrentPatchName() const noexcept { return currentPatchName; }

    /// Backing file for the current patch. Empty `juce::File{}` when the
    /// current state is Init.
    juce::File GetCurrentPatchFile() const noexcept { return currentPatchFile; }

    /// Init / Factory / User. Drives UI gating for the action buttons.
    CurrentSource GetCurrentSource() const noexcept { return currentSource; }

    /// True only when the current patch is a writable user-owned file on disk.
    /// The UI uses this to enable the Delete button and decide whether Save
    /// can overwrite in place (vs. falling through to Save As).
    bool IsCurrentPatchUserOwned() const noexcept { return currentSource == CurrentSource::User; }

    /// True when the current patch was loaded from the factory bank. Used to
    /// gate Delete and to colour the patch-name display.
    bool IsCurrentPatchFactory() const noexcept { return currentSource == CurrentSource::Factory; }

    //==========================================================================
    // Name helpers.
    //==========================================================================

    /// Strips filesystem-illegal characters from `requestedName`, trims
    /// whitespace, and falls back to "Untitled" if the result is empty.
    /// Exposed so the Save As dialog can live-preview the sanitized name as
    /// the user types. Pure — no I/O.
    static juce::String SanitizeFilename(const juce::String &requestedName);

    //==========================================================================
    // Dirty tracking.
    //==========================================================================

    /// True when any non-excluded APVTS param has changed since the last
    /// successful Save / Load / Init. The title-header reads this to render
    /// the trailing `*` next to the patch name.
    bool IsDirty() const noexcept { return isDirty.load(std::memory_order_acquire); }

private:
    /// APVTS::Listener override. Fires on whichever thread changed the
    /// param — audio thread for host automation, message thread for GUI
    /// edits — so the dirty flag has to be atomic. Short-circuits while
    /// `suppressDirty` is set so programmatic loads don't trip themselves.
    void parameterChanged(const juce::String &paramID, float newValue) override;

    static constexpr const char *patchFileExtension = ".dlbs";
    static constexpr const char *patchRootTagName   = "DLBSPatch";

    /// True for paramIDs excluded from patch save / load. Currently just
    /// `tempo_fallback_bpm` — a per-session setup param.
    static bool IsExcludedFromPatch(const juce::String &paramID) noexcept;

    /// True for paramIDs that RandomizeAll() leaves untouched (separate from
    /// the patch-exclusion list because RandomizeAll also skips
    /// pitch_bend_range and the five master-section safety params, which
    /// are forced to safe values rather than randomised).
    static bool IsExcludedFromRandomize(const juce::String &paramID) noexcept;

    /// Resolves the user patches directory path. Pure path computation, no I/O.
    static juce::File ResolveUserPatchesDirectory();

    /// Resolves the factory patches directory inside the plugin bundle's
    /// Resources/. Pure path computation.
    static juce::File ResolveFactoryPatchesDirectory();

    /// Scans `dir` for *.dlbs files, appending each as a PatchInfo with the
    /// given source. No-op if `dir` doesn't exist.
    void ScanDirectoryInto(const juce::File &dir, Source source, std::vector<PatchInfo> &out) const;

    /// Computes a unique, filesystem-safe destination File under the user
    /// directory for `requestedName`. Strips illegal chars, falls back to
    /// "Untitled" on empty input, and appends " 2", " 3", … until the name
    /// doesn't clash with any existing user or factory patch.
    juce::File MakeUniqueUserPatchFile(const juce::String &requestedName) const;

    /// Updates the cached "currently loaded" trio in a single call. Internal
    /// helper used by every load / save / delete path.
    void SetCurrent(CurrentSource source, const juce::File &file, const juce::String &name);

    /// Registers / unregisters this object as a listener on every non-excluded
    /// APVTS param. RegisterParameterListeners is called once from Init();
    /// the destructor unregisters.
    void RegisterParameterListeners();
    void UnregisterParameterListeners();

    /// Forces the dirty flag back to clean. Called at the end of every Load /
    /// Save path so the listener callbacks fired during the operation don't
    /// leave the patch in a falsely-dirty state.
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
