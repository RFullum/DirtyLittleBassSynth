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
    : public juce::AudioProcessor
{
public:
    DirtyLittleBassSynthAudioProcessor();
    ~DirtyLittleBassSynthAudioProcessor() override = default;

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

    float                GetMasterGainReductionDb() const noexcept { return masterChain.GetGainReductionDb(); }
    const ScopeBuffer   &GetScopeBuffer()           const noexcept { return scopeBuffer; }
    const TempoSnapshot &GetTempoSnapshot()         const noexcept { return tempoSnapshot; }
    MidiLearnManager    &GetMidiLearnManager()            noexcept { return midiLearnManager; }
    PatchManager        &GetPatchManager()                noexcept { return patchManager; }

    void SaveMidiLearnMappings();
    
    juce::AudioProcessorValueTreeState parameters;

    juce::AudioBuffer<float> outputLevelBuffer;

private:
    void RegisterMidiLearnableParams();
    void LoadMidiLearnMappings();
    
    std::atomic<float>* oscMorphParameter;
    std::atomic<float>* subOscMorphParameter;
    std::atomic<float>* subGainParameter;
    std::atomic<float>* subOctaveParameter;
    std::atomic<float>* pitchBendParameter;
    
    std::atomic<float>* ampAttackParameter;
    std::atomic<float>* ampDecayParameter;
    std::atomic<float>* ampSustainParameter;
    std::atomic<float>* ampReleaseParameter;
    
    std::atomic<float>* foldbackDistParameter;

    std::atomic<float>* ringModPitchParameter;
    std::atomic<float>* ringToneParameter;
    std::atomic<float>* ringModMixParameter;
    
    std::atomic<float>* freqShiftPitchParameter;
    std::atomic<float>* freqShiftMixParameter;
    
    std::atomic<float>* sAndHPitchParameter;
    std::atomic<float>* sAndHMixParameter;
    
    std::atomic<float>* filterCutoffParameter;
    std::atomic<float>* filterResonanceParameter;
    std::atomic<float>* filterSelectorParameter;
    
    std::atomic<float>* filtEnvAttackParameter;
    std::atomic<float>* filtEnvDecayParameter;
    std::atomic<float>* filtEnvSustainParameter;
    std::atomic<float>* filtEnvReleaseParameter;
    std::atomic<float>* filtEnvAmtCOParameter;
    std::atomic<float>* filtEnvAmtResParameter;
    
    std::atomic<float>* filtLFOFreqParameter;
    std::atomic<float>* filtLFOAmtParameter;
    std::atomic<float>* filtLFOShapeParameter;

    std::atomic<float>* filtLFOSyncOnParameter;
    std::atomic<float>* filtLFOSyncDivParameter;
    
    std::atomic<float>* portaTimeParameter;
    std::atomic<float>* portaOnParameter;
    std::atomic<float>* portaLegatoParameter;
    
    std::atomic<float>* masterGainParameter;

    std::atomic<float>* masterWideParameter;
    std::atomic<float>* monoBelowFreqParameter;

    std::atomic<float>* limiterOnParameter;
    std::atomic<float>* limiterCeilingParameter;

    std::atomic<float>* tempoFallbackBpmParameter;

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



