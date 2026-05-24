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

// audio thread calls HandleControllerMessage for each incoming CC
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

    int  RegisterParam(juce::AudioProcessorValueTreeState &apvts, const juce::String &paramID);
    void HandleControllerMessage(int ccNumber, int ccValue);

    State GetState()             const noexcept { return state.load           (std::memory_order_acquire); }
    int   GetArmedParamIndex()   const noexcept { return armedParamIndex.load (std::memory_order_acquire); }

    void EnterListening() noexcept;
    void ExitLearn() noexcept;
    void ArmParam(int paramIndex) noexcept;

    void SetMapping(int ccNumber, int paramIndex) noexcept;
    void RegisterDefaultMapping(int ccNumber, int paramIndex) noexcept;
    void UnmapParam(int paramIndex) noexcept;
    void ClearAllMappings() noexcept;

    int GetParamForCc(int ccNumber) const noexcept;
    int GetFirstCcForParam(int paramIndex) const noexcept;
    bool ConsumeAppliedFromCcFlag(int paramIndex) noexcept;

    int                          GetNumParams()                    const noexcept { return (int) params.size(); }
    juce::String                 GetParamID  (int paramIndex)      const;
    juce::RangedAudioParameter*  GetParam    (int paramIndex)      const noexcept;
    int                          GetParamIndexById(const juce::String &paramID) const;

    juce::String SerialiseMappings() const;
    void RestoreMappings(const juce::String &serialised);
    
    bool IsDirty()       const noexcept { return dirty.load (std::memory_order_acquire); }
    void ClearDirtyFlag()      noexcept { dirty.store(false, std::memory_order_release); }

private:
    static constexpr int numCcSlots         = 128;
    static constexpr int maxLearnableParams = 128;   // upper bound for appliedFromCc flags

    struct Entry
    {
        juce::RangedAudioParameter *param = nullptr;
        juce::String                paramID;
    };

    struct DefaultMapping
    {
        int cc         = -1;
        int paramIndex = -1;
    };

    std::vector<Entry>                       params;
    std::array<std::atomic<int>, numCcSlots> ccToParamIndex;   // -1 = unmapped
    std::vector<DefaultMapping>              defaultMappings;

    std::atomic<State> state            { State::Idle };
    std::atomic<int>   armedParamIndex  { -1 };
    std::atomic<bool>  dirty            { false };

    // Per-param flag set by HandleControllerMessage immediately before the
    // setValueNotifyingHost call, consumed by the CC-echo listener so it can
    // skip echoing the value the controller just sent.
    std::array<std::atomic<bool>, maxLearnableParams> appliedFromCc {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiLearnManager)
};
