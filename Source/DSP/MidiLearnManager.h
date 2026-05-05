/*
  ==============================================================================

    MidiLearnManager.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <array>
#include <vector>

//==============================================================================

/// Centralised CC → parameter routing with a learn-mode state machine.
///
/// The audio thread calls HandleControllerMessage for each incoming CC; if
/// the manager is in the Armed state the CC is bound to the armed parameter,
/// otherwise the CC is looked up in the mapping table and (if mapped) drives
/// the corresponding parameter via setValueNotifyingHost.
///
/// The lookup table is `cc → paramIndex`, one direction. Several CCs can map
/// to the same parameter (a knob and a fader controlling cutoff together);
/// each CC controls at most one parameter. Re-binding a CC to a new param
/// silently replaces its previous binding.
///
/// State transitions and table mutations all use atomics, so the audio thread
/// can read freely while the UI thread arms/unmaps without locks.
class MidiLearnManager
{
public:
    enum class State
    {
        Idle,        // not learning
        Listening,   // learn mode active, no param armed
        Armed        // a param has been clicked, waiting for the next CC
    };

    MidiLearnManager();

    //==========================================================================
    // Setup — call from PluginProcessor's constructor.
    //==========================================================================

    /// Registers a learnable parameter from the APVTS by its parameterID.
    /// Returns the assigned parameter index (>= 0) or -1 if the param wasn't
    /// found. The index is stable for the lifetime of the manager.
    int RegisterParam(juce::AudioProcessorValueTreeState &apvts, const juce::String &paramID);

    //==========================================================================
    // Audio thread.
    //==========================================================================

    /// Called from processBlock for each incoming CC. If in Armed state, binds
    /// the CC to the currently armed param and returns to Listening. Otherwise
    /// looks up the CC in the table and (if mapped) updates the corresponding
    /// parameter.
    void HandleControllerMessage(int ccNumber, int ccValue);

    //==========================================================================
    // Learn-mode state (UI thread).
    //==========================================================================

    State GetState()             const noexcept { return state.load           (std::memory_order_acquire); }
    int   GetArmedParamIndex()   const noexcept { return armedParamIndex.load (std::memory_order_acquire); }

    /// Enter learn mode: state → Listening (or Armed → Listening if a param
    /// was previously armed). Idempotent.
    void EnterListening() noexcept;

    /// Leave learn mode entirely: state → Idle.
    void ExitLearn() noexcept;

    /// Arm a parameter for the next incoming CC. Requires state to be in
    /// learn mode (Listening or Armed). No-op if paramIndex is out of range.
    void ArmParam(int paramIndex) noexcept;

    //==========================================================================
    // Mapping queries / mutations (UI thread).
    //==========================================================================

    /// Sets a CC → param binding directly (used for hardcoded defaults and
    /// during persistence load). Pass paramIndex = -1 to clear the slot.
    void SetMapping(int ccNumber, int paramIndex) noexcept;

    /// Removes every CC that points to the given param. No-op if paramIndex
    /// is out of range.
    void UnmapParam(int paramIndex) noexcept;

    /// Wipes the entire mapping table.
    void ClearAllMappings() noexcept;

    /// Returns the param index controlled by the given CC, or -1 if unmapped.
    int GetParamForCc(int ccNumber) const noexcept;

    /// Returns the first CC bound to the given param (lowest CC#), or -1 if
    /// the param has no CC binding. (A param can have multiple CCs; this
    /// returns the lowest for use in CC# badge display.)
    int GetFirstCcForParam(int paramIndex) const noexcept;

    //==========================================================================
    // Param info (UI thread).
    //==========================================================================

    int                          GetNumParams()                    const noexcept { return (int) params.size(); }
    juce::String                 GetParamID  (int paramIndex)      const;
    juce::RangedAudioParameter*  GetParam    (int paramIndex)      const noexcept;
    int                          GetParamIndexById(const juce::String &paramID) const;

    //==========================================================================
    // Persistence.
    //==========================================================================

    /// Serialises the current mappings into a "cc:paramID|cc:paramID|..." string
    /// suitable for storage via juce::PropertiesFile::setValue.
    juce::String SerialiseMappings() const;

    /// Restores mappings from a string produced by SerialiseMappings. Existing
    /// mappings are cleared first. Unknown paramIDs are ignored silently
    /// (e.g., if a param was renamed between plugin versions).
    void RestoreMappings(const juce::String &serialised);

private:
    static constexpr int numCcSlots = 128;

    struct Entry
    {
        juce::RangedAudioParameter *param = nullptr;
        juce::String                paramID;
    };

    std::vector<Entry>                       params;
    std::array<std::atomic<int>, numCcSlots> ccToParamIndex;   // -1 = unmapped

    std::atomic<State> state            { State::Idle };
    std::atomic<int>   armedParamIndex  { -1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiLearnManager)
};
