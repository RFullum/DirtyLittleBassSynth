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
class DirtyLittleBassSynthAudioProcessor  : public AudioProcessor
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

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Parameter members
    AudioProcessorValueTreeState parameters;
    
    // WaveDrawing
    AudioBuffer<float> mainOscVisualBuffer;
    AudioBuffer<float> subOscVisualBuffer;
    AudioBuffer<float> lfoOscVisualBuffer;

private:
    // Parameter members
    
    // Oscillator parameter members
    std::atomic<float>* oscMorphParameter;
    std::atomic<float>* subOscMorphParameter;
    std::atomic<float>* subGainParameter;
    std::atomic<float>* subOctaveParameter;
    std::atomic<float>* pitchBendParameter;
    
    // Amp ADSR parameters
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
    
    // Filter ADSR Parameters
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
    
    // Instance of SynthesiZer class
    Synthesiser synth;
    int voiceCount = 1;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirtyLittleBassSynthAudioProcessor)
};
