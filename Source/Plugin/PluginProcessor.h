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

    float getOutLevel();

    /// Latest gain reduction in dB from the master limiter (0 when not limiting).
    /// Polled by the editor's timer to drive the GR meter.
    float GetMasterGainReductionDb() const noexcept { return masterChain.GetGainReductionDb(); }

    /// Read-only handle to the post-limiter sample feed used by the scope display.
    const ScopeBuffer &GetScopeBuffer() const noexcept { return scopeBuffer; }

    /// Read-only handle to the tempo snapshot updated each block from the host's
    /// playhead (or the fallback BPM param when no host transport is reporting).
    const TempoSnapshot &GetTempoSnapshot() const noexcept { return tempoSnapshot; }

    /// Mutable handle to the MIDI Learn manager. UI uses it to enter/exit
    /// learn mode, arm params, query mappings, and trigger Clear All.
    MidiLearnManager &GetMidiLearnManager() noexcept { return midiLearnManager; }

    /// Mutable handle to the patch manager. UI uses it to populate the patch
    /// list and (later) trigger save / load / delete.
    PatchManager &GetPatchManager() noexcept { return patchManager; }

    juce::AudioProcessorValueTreeState parameters;

    juce::AudioBuffer<float> outputLevelBuffer;

private:
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

    void RegisterMidiLearnableParams();
    void LoadMidiLearnMappings();

public:
    /// Persists the current MIDI Learn mappings to the user-settings file.
    /// Called from the editor's timer when the manager's dirty flag is set,
    /// so file I/O happens on the message thread, not audio.
    void SaveMidiLearnMappings();

private:

    juce::Synthesiser synth;
    int voiceCount = 1;

    // Typed pointers to voices owned by `synth`. Populated in the constructor so
    // processBlock can avoid dynamic_cast on the audio thread.
    std::vector<BassSynthVoice*> typedVoices;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirtyLittleBassSynthAudioProcessor)
};



