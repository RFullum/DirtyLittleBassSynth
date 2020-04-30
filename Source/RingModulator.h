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

class RingMod
{
public:
    /// Sets sampleRates and set up wavetables
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        setUpWavetables();
    }
    
    /// Takes the note frequency to set wavetable increment
    void modFreq(float fqncy, std::atomic<float>* offset)
    {
        float modFreq = fqncy * *offset;

        wtSine.setIncrement(modFreq);
        wtSquare.setIncrement(modFreq);
    }
    
    void setRingToneSlider(std::atomic<float>* toneSlider)
    {
        ringToneSlider = toneSlider;
    }
    
    /// Process wavetable: toneSlider controls amount of Sin/Square wave
    float process() //(std::atomic<float>* toneSlider)
    {
        float sinVal = wtSine.process();
        float sqVal = wtSquare.process();
        float outVal = toneControl.dryWetMix(sinVal, sqVal, ringToneSlider);
        
        return outVal;
    }
    
private:
    void setUpWavetables()
    {
        // Sets wavetable samplerate
        wtSine.setSampleRate(sampleRate);
        wtSquare.setSampleRate(sampleRate);
        
        // Populates wavetables
        wtSine.populateWavetable();
        wtSquare.populateWavetable();
    }
    
    // Wavetables for Mod Oscs
    Wavetable wtSine;
    SquareWavetable wtSquare;
    
    // DryWet instance
    DryWet toneControl;
    
    // Member Variables
    float sampleRate;
    std::atomic<float>* ringToneSlider;
};
