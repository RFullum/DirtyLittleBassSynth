#pragma once

#include <JuceHeader.h>
#include "BassSynthVoice.h"
#include "MasterChain.h"
#include "ScopeBuffer.h"
#include "TempoSnapshot.h"
#include "MidiLearnManager.h"
#include "PatchManager.h"

//==============================================================================

class DirtyLittleBassSynthAudioProcessor
    : public  juce::AudioProcessor
    , private juce::AudioProcessorValueTreeState::Listener
{
public:
    DirtyLittleBassSynthAudioProcessor();
    ~DirtyLittleBassSynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor()    override;
    bool                        hasEditor() const override;

    const juce::String getName() const override;

    bool   acceptsMidi()          const override;
    bool   producesMidi()         const override;
    bool   isMidiEffect()         const override;
    double getTailLengthSeconds() const override;

    int                getNumPrograms()             override;
    int                getCurrentProgram()          override;
    void               setCurrentProgram(int index) override;
    const juce::String getProgramName(int index)    override;
    void               changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    /// Last-sample snapshots from the active voice, published once per audio
    /// block for the FilterPanel's animated visualizer. UI thread reads these
    /// to draw the modulated cutoff/res position on the response curve.
    std::atomic<float> filtEnvDisplayValue { 0.0f };  // 0..1   (filter ADSR)
    std::atomic<float> filtLFODisplayValue { 0.0f };  // -1..1  (post click-env)

    float                GetMasterGainReductionDb() const noexcept { return masterChain.GetGainReductionDb(); }
    const ScopeBuffer   &GetScopeBuffer()           const noexcept { return scopeBuffer; }
    const TempoSnapshot &GetTempoSnapshot()         const noexcept { return tempoSnapshot; }
    MidiLearnManager    &GetMidiLearnManager()            noexcept { return midiLearnManager; }
    PatchManager        &GetPatchManager()                noexcept { return patchManager; }

    void SaveMidiLearnMappings();

    /// Stops every active voice immediately (no envelope tail). Safe to call
    /// from the UI thread — Synthesiser guards note state with its own lock.
    void MidiPanic();

    /// Global tooltip on/off preference. Persisted per-user via
    /// `juce::ApplicationProperties` (same store as MIDI Learn mappings).
    bool GetTooltipsEnabled() const;
    void SetTooltipsEnabled(bool enabled);
    
    bool GetCcEchoEnabled() const;
    void SetCcEchoEnabled(bool enabled);
    
    juce::AudioProcessorValueTreeState parameters;

    juce::AudioBuffer<float> outputLevelBuffer;

private:
    void RegisterMidiLearnableParams();
    void LoadMidiLearnMappings();

    /// APVTS Listener entry point — used for CC Echo.
    void parameterChanged(const juce::String &parameterID, float newValue) override;
    void AttachCcEchoListeners();
    void DetachCcEchoListeners();

    /// CC# → next-pending-value (0..127), or -1 for "nothing pending".
    std::array<std::atomic<int>, 128> pendingCcEcho;
    std::atomic<bool>                 ccEchoEnabled { true };
    juce::StringArray                 ccEchoListenedParamIDs;
    
    std::atomic<float>* oscMorphParameter    = nullptr;
    std::atomic<float>* subOscMorphParameter = nullptr;
    std::atomic<float>* subGainParameter     = nullptr;
    std::atomic<float>* subOctaveParameter   = nullptr;
    std::atomic<float>* pitchBendParameter   = nullptr;

    std::atomic<float>* ampAttackParameter  = nullptr;
    std::atomic<float>* ampDecayParameter   = nullptr;
    std::atomic<float>* ampSustainParameter = nullptr;
    std::atomic<float>* ampReleaseParameter = nullptr;

    std::atomic<float>* foldbackDistParameter = nullptr;

    std::atomic<float>* ringModPitchParameter = nullptr;
    std::atomic<float>* ringToneParameter     = nullptr;
    std::atomic<float>* ringModMixParameter   = nullptr;

    std::atomic<float>* freqShiftPitchParameter = nullptr;
    std::atomic<float>* freqShiftMixParameter   = nullptr;

    std::atomic<float>* sAndHPitchParameter = nullptr;
    std::atomic<float>* sAndHMixParameter   = nullptr;

    std::atomic<float>* filterCutoffParameter    = nullptr;
    std::atomic<float>* filterResonanceParameter = nullptr;
    std::atomic<float>* filterSelectorParameter  = nullptr;

    std::atomic<float>* filtEnvAttackParameter  = nullptr;
    std::atomic<float>* filtEnvDecayParameter   = nullptr;
    std::atomic<float>* filtEnvSustainParameter = nullptr;
    std::atomic<float>* filtEnvReleaseParameter = nullptr;
    std::atomic<float>* filtEnvAmtCOParameter   = nullptr;
    std::atomic<float>* filtEnvAmtResParameter  = nullptr;

    std::atomic<float>* filtLFOFreqParameter  = nullptr;
    std::atomic<float>* filtLFOAmtParameter   = nullptr;
    std::atomic<float>* filtLFOShapeParameter = nullptr;

    std::atomic<float>* filtLFOSyncOnParameter  = nullptr;
    std::atomic<float>* filtLFOSyncDivParameter = nullptr;

    std::atomic<float>* portaTimeParameter   = nullptr;
    std::atomic<float>* portaOnParameter     = nullptr;
    std::atomic<float>* portaLegatoParameter = nullptr;

    std::atomic<float>* masterGainParameter = nullptr;

    std::atomic<float>* masterWideParameter    = nullptr;
    std::atomic<float>* monoBelowFreqParameter = nullptr;

    std::atomic<float>* limiterOnParameter      = nullptr;
    std::atomic<float>* limiterCeilingParameter = nullptr;

    std::atomic<float>* tempoFallbackBpmParameter = nullptr;

    MasterChain      masterChain;
    ScopeBuffer      scopeBuffer;
    TempoSnapshot    tempoSnapshot;
    MidiLearnManager midiLearnManager;
    PatchManager     patchManager { parameters };

    juce::ApplicationProperties applicationProperties;

private:

    juce::Synthesiser synth;
    int voiceCount = 1;

    // Typed pointers to voices owned by `synth`. Populated in the constructor so
    // processBlock can avoid dynamic_cast on the audio thread.
    std::vector<BassSynthVoice*> typedVoices;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirtyLittleBassSynthAudioProcessor)
};



