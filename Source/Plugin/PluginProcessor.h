/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthesiserStarting.h"

//==============================================================================
/**
*/
class DirtyLittleBassSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DirtyLittleBassSynthAudioProcessor();
    ~DirtyLittleBassSynthAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    /// Gets transport/bpm/etc info from host
    //void updateCurrentTimeInfoFromHost();
    
    float getOutLevel();
    
    // Parameter members
    juce::AudioProcessorValueTreeState parameters;
    
    // WaveDrawing
    juce::AudioBuffer<float> mainOscVisualBuffer;
    juce::AudioBuffer<float> subOscVisualBuffer;
    juce::AudioBuffer<float> lfoOscVisualBuffer;
    
    // Metering
    juce::AudioBuffer<float> outputLevelBuffer;

private:
    // Parameter members
    
    // Oscillator parameter members
    std::atomic<float>* oscMorphParameter;
    std::atomic<float>* subOscMorphParameter;
    std::atomic<float>* subGainParameter;
    std::atomic<float>* subOctaveParameter;
    std::atomic<float>* pitchBendParameter;
    
    // Amp juce::ADSR parameters
    std::atomic<float>* ampAttackParameter;
    std::atomic<float>* ampDecayParameter;
    std::atomic<float>* ampSustainParameter;
    std::atomic<float>* ampReleaseParameter;
    
    // Foldback distortion parameters
    std::atomic<float>* foldbackDistParameter;
    
    // DryWet parameters
    // std::atomic<float>* dryWetMixParameter;
    
    // RingMod parameters
    std::atomic<float>* ringModPitchParameter;
    std::atomic<float>* ringToneParameter;
    std::atomic<float>* ringModMixParameter;
    
    // Frequency Shift parameters
    std::atomic<float>* freqShiftPitchParameter;
    std::atomic<float>* freqShiftMixParameter;
    
    // Sample and Hold parameters
    std::atomic<float>* sAndHPitchParameter;
    std::atomic<float>* sAndHMixParameter;
    
    // Filter parameters
    std::atomic<float>* filterCutoffParameter;
    std::atomic<float>* filterResonanceParameter;
    std::atomic<float>* filterSelectorParameter;
    
    // Filter juce::ADSR Parameters
    std::atomic<float>* filtEnvAttackParameter;
    std::atomic<float>* filtEnvDecayParameter;
    std::atomic<float>* filtEnvSustainParameter;
    std::atomic<float>* filtEnvReleaseParameter;
    std::atomic<float>* filtEnvAmtCOParameter;
    std::atomic<float>* filtEnvAmtResParameter;
    
    // Filter LFO Parameters
    std::atomic<float>* filtLFOFreqParameter;
    std::atomic<float>* filtLFOAmtParameter;
    std::atomic<float>* filtLFOShapeParameter;
    
    // Portamento Parameters
    std::atomic<float>* portaTimeParameter;
    
    // Master Gain
    std::atomic<float>* masterGainParameter;
    
    // Transport Info
    //juce::AudioPlayHead::CurrentPositionInfo playHeadInfo;
    
    // Instance of SynthesiZer class
    juce::Synthesiser synth;
    int voiceCount = 1;

    // Typed pointers to voices owned by `synth`. Populated in the constructor so
    // processBlock can avoid dynamic_cast on the audio thread.
    std::vector<MySynthVoice*> typedVoices;
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirtyLittleBassSynthAudioProcessor)
};



//==============================================================================
//==============================================================================
//==============================================================================
