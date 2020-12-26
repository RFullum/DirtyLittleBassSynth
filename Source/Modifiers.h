/*
  ==============================================================================

    Modifiers.h
    Created: 29 Apr 2020 10:30:04pm
    Author:  B150987

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Wavetable.h"
#include "DryWet.h"
#include "OscillatorParameterControls.h"

//
// Audio Modifiers: Ring Modulator, Frequency Shifter, Sample and Hold Distortion
//

/// Ring Modulation Class
class RingMod
{
public:
    // Destructo!
    virtual ~RingMod();
    
    /// Sets sampleRates and set up wavetables
    virtual void setSampleRate(float SR);
    
    /// Takes the note frequency to set wavetable increment
    virtual void modFreq(float fqncy, std::atomic<float>* offset);
    
    // Setter for ringToneSlider value
    void setRingToneSlider(std::atomic<float>* toneSlider);
    
    /// Process wavetable: toneSlider controls amount of Sin/Square wave
    virtual float process();
    
protected:
    // Member variables
    float sampleRate;
    float modFrequency;
    
private:
    /// sets sampleRate for member wavetables and calls populate function
    void setUpWavetables();
    
    /// Sets the playback frequency for wavetables
    void setRingIncrement();
    
    /// Calculate ring modulation: calls wavetable process functions, morphs their shapes, and returns sample value
    float ringModProcess();
    
    // Wavetables for Mod Oscs
    Wavetable wtSine;
    SquareWavetable wtSquare;
    
    // DryWet instance
    DryWet toneControl;
    
    // Member Variables
    //std::atomic<float>* ringToneSlider;
    float ringToneSlider;
};



//============================================================

/// Frequency Shifter Class (child of RingMod class)
class FrequencyShifter : public RingMod
{
public:
    /// Sets sampleRate for frequency shifter wavetables and populates wavetables
    void setSampleRate(float SR) override;
    
    /// Takes the note frequency to set wavetable increment
    void modFreq(float fqncy, std::atomic<float>* offset) override;
    
    /// Follows main oscillator morph to morph between Sine/Spike/Saw wavetables
    void oscMorph(std::atomic<float>* morph);
    
    /// Does the frequency shifting and returns the sample value
    float process() override;
    
private:
    /// Sets sampleRate for member wavetables and populates them
    void setUpFreqShiftWavetables();
    
    /// sets playback frequency
    void setFreqShiftIncrement();
    
    /// Does the frequency shifting and returns the sample value
    float freqShiftProcess();
    
    // Wavetable instances
    Wavetable wtSine;
    SpikeWavetable wtSpike;
    SawWavetable wtSaw;
    
    // OscillatorParameterControl instance
    OscParamControl oscParamControl;
    
    // Member Variables
    float sineLevel;
    float spikeLevel;
    float sawLevel;
};


//============================================================

/// Sample and Hold distortion class: Child of RingMod class
class SampleAndHold : public RingMod
{
public:
    /// Constructor
    SampleAndHold();
    
    /// Sets sampleRate for member wavetables and populates them
    void setSampleRate(float SR) override;
    
    /// sets playback frequency
    void modFreq(float fqncy, std::atomic<float>* offset) override;
    
    /**
     Takes in current oscillator sample value and processes through S&H
     */
    float processSH(float oscSampleValIn);
    
private:
    /// Sets sampleRate for member wavetables and populates them
    void setSampHoldWavetables();
    
    /// sets playback frequency
    void setSampHoldIncrement();
    
    /// Processes S&H distortion and returns wave value
    float sampleHoldProcess();
    
    // Wavetable instances
    SquareWavetable wtSampHold;
    
    // Member Variables
    float oscSampleVal;
    float holdSampleVal;
};
