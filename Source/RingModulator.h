/*
  ==============================================================================

    RingModulator.h
    Created: 29 Apr 2020 10:30:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "Wavetable.h"

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
    
    //
    //  NEEDS DRY/WET (TONE KNOB) ON THE SINE/SQUARE
    //
    /// Process wavetable
    float process()
    {
        return wtSine.process();
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
    
    // Member Variables
    float sampleRate;
};
