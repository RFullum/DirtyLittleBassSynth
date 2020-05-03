/*
  ==============================================================================

    RingModulator.h
    Created: 29 Apr 2020 10:30:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "Wavetable.h"
#include "DryWet.h"
#include "OscillatorParameterControls.h"

class RingMod
{
public:
    // Destructo!
    virtual ~RingMod()
    {
        
    }
    
    /// Sets sampleRates and set up wavetables
    virtual void setSampleRate(float SR)
    {
        sampleRate = SR;
        setUpWavetables();
    }
    
    /// Takes the note frequency to set wavetable increment
    virtual void modFreq(float fqncy, std::atomic<float>* offset)
    {
        modFrequency = fqncy * *offset;
        setRingIncrement();
    }
    
    // Setter for ringToneSlider value
    void setRingToneSlider(std::atomic<float>* toneSlider)
    {
        ringToneSlider = *toneSlider;
    }
    
    /// Process wavetable: toneSlider controls amount of Sin/Square wave
    virtual float process()
    {
        return ringModProcess();
    }
    
protected:
    // Member variables
    float sampleRate;
    float modFrequency;
    
private:
    /// sets sampleRate for member wavetables and calls populate function
    void setUpWavetables()
    {
        // Sets wavetable samplerate
        wtSine.setSampleRate(sampleRate);
        wtSquare.setSampleRate(sampleRate);
        
        // Populates wavetables
        wtSine.populateWavetable();
        wtSquare.populateWavetable();
    }
    
    /// Sets the playback frequency for wavetables
    void setRingIncrement()
    {
        wtSine.setIncrement(modFrequency);
        wtSquare.setIncrement(modFrequency);
    }
    
    float ringModProcess()
    {
        float sinVal = wtSine.process();
        float sqVal = wtSquare.process();
        float outVal = toneControl.dryWetMix(sinVal, sqVal, ringToneSlider);
        
        return outVal;
    }
    
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

class FrequencyShifter : public RingMod
{
public:
    /// Sets sampleRate for frequency shifter wavetables and populates wavetables
    void setSampleRate(float SR) override
    {
        sampleRate = SR;
        setUpFreqShiftWavetables();
    }
    
    /// Takes the note frequency to set wavetable increment
    void modFreq(float fqncy, std::atomic<float>* offset) override
    {
        modFrequency = fqncy * *offset;
        setFreqShiftIncrement();
    }
    
    /// Follows main oscillator morph to morph between Sine/Spike/Saw wavetables
    void oscMorph(std::atomic<float>* morph)
    {
        sineLevel = oscParamControl.sinMorphGain(morph);
        spikeLevel = oscParamControl.spikeMorphGain(morph);
        sawLevel = oscParamControl.sawMorphGain(morph);
    }
    
    /// Does the frequency shifting and returns the sample value
    float process() override
    {
        return freqShiftProcess();
    }
    
private:
    /// Sets sampleRate for member wavetables and populates them
    void setUpFreqShiftWavetables()
    {
        wtSine.setSampleRate(sampleRate);
        wtSaw.setSampleRate(sampleRate);
        wtSpike.setSampleRate(sampleRate);
        
        wtSine.populateWavetable();
        wtSaw.populateWavetable();
        wtSpike.populateWavetable();
    }
    
    /// sets playback frequency
    void setFreqShiftIncrement()
    {
        wtSine.setIncrement(modFrequency);
        wtSaw.setIncrement(modFrequency);
        wtSpike.setIncrement(modFrequency);
    }
    
    /// Does the frequency shifting and returns the sample value
    float freqShiftProcess()
    {
        float sinVal = wtSine.process() * sineLevel;
        float spikeVal = wtSpike.process() * spikeLevel;
        float sawVal = wtSaw.process() * sawLevel;
        
        return ( sinVal + spikeVal + sawVal ) * 0.5f;
    }
    
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

class SampleAndHold : public RingMod
{
public:
    /// Sets sampleRate for member wavetables and populates them
    void setSampleRate(float SR) override
    {
        sampleRate = SR;
        setSampHoldWavetables();
    }
    
    /// sets playback frequency
    void modFreq(float fqncy, std::atomic<float>* offset) override
    {
        modFrequency = fqncy * *offset;
        setSampHoldIncrement();
    }
    
    /**
     Takes in current oscillator sample value and processes through S&H
     */
    float processSH(float oscSampleValIn)
    {
        oscSampleVal = oscSampleValIn;
        return sampleHoldProcess();
    }
    
private:
    /// Sets sampleRate for member wavetables and populates them
    void setSampHoldWavetables()
    {
        wtSampHold.setSampleRate(sampleRate);
        wtSampHold.populateWavetable();
    }
    
    /// sets playback frequency
    void setSampHoldIncrement()
    {
        wtSampHold.setIncrement(modFrequency);
    }
    
    float sampleHoldProcess()
    {
        float outVal;
        float sampHoldVal = wtSampHold.process();
        
        if (sampHoldVal >= 0.0f)
        {
            outVal = oscSampleVal;
            holdSampleVal = outVal;
        }
        else
        {
            outVal = holdSampleVal;
        }
            
        return outVal;
    }
    
    // Wavetable instances
    SquareWavetable wtSampHold;
    
    // Member Variables
    float oscSampleVal;
    float holdSampleVal= 0.0f;
};
