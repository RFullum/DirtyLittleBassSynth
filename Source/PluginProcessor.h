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
class Wavetable5AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Wavetable5AudioProcessor();
    ~Wavetable5AudioProcessor();

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

private:
    // Parameter members
    AudioProcessorValueTreeState parameters;
    
    // Oscillator parameter members
    std::atomic<float>* oscMorphParameter;
    std::atomic<float>* subOscMorphParameter;
    std::atomic<float>* subGainParameter;
    std::atomic<float>* subOctaveParameter;
    
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
    
    // Instance of SynthesiZer class
    Synthesiser synth;
    int voiceCount = 16;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wavetable5AudioProcessor)
};
