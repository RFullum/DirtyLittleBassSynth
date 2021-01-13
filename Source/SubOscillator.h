/*
  ==============================================================================

    SubOscillator.h
    Created: 28 Apr 2020 2:17:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once
#include "Wavetable.h"
#include "OscillatorParameterControls.h"

/// Creates a sub oscillator using Sine, Square, and Saw wavetables
class SubOsc
{
public:
    /// Sets sampleRate of sub oscillators
    void setSampleRate(float SR);
    
    /// Populates values to the sub oscillator wavetables
    void populateWavetable();
    
    
    /**
     Sets playback frequency of sub oscillator scaled 0 to 2 octaves below
     noteFreq is the playback note frequency. denom is the denominator to chose sub octave: 1, 2, or 4
     */
    void setIncrement(float noteFreq, int denom);
    
    /**
     Playback function: gain arguments come from SubOscParamControl class member functions to morph between the three
     wave shapes: sinSubGain( ), squareSubGain( ), and sawSubGain( )
     */
    float process(float sinGain, float squareGain, float sawGain);
    
    /// Returns the sample value of the wavetable at the index
    float getSquareWavetableValue(int index);
    
    
private:
    // Wavetable instances
    Wavetable sinSub;
    SquareWavetable squareSub;
    SawWavetable sawSub;

};
