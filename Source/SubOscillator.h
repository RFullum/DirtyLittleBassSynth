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

class SubOsc
{
public:
    /// Sets sampleRate of sub oscillators
    void setSampleRate(float SR)
    {
        sinSub.setSampleRate(SR);
        squareSub.setSampleRate(SR);
        sawSub.setSampleRate(SR);
    }
    
    /// Populates values to the sub oscillator wavetables
    void populateWavetable()
    {
        sinSub.populateWavetable();
        squareSub.populateWavetable();
        sawSub.populateWavetable();
    }
    
    /*
     subDenominator should be made to a float instead of type casted.
     do this in a setter function that takes the parameter from the tree
     with a min of 1 (same octave) and a max of 3 (two octaves below).
     When the value is 3 it needs to be 4 to get proper denominator
     */
    /// Sets playback frequency of sub oscillator scaled 0 to 2 octaves below
    void setIncrement(float noteFreq, int denom)
    {
        float subFreq = noteFreq / (float)denom;
        
        sinSub.setIncrement(subFreq);
        squareSub.setIncrement(subFreq);
        sawSub.setIncrement(subFreq);
    }
    
    /// Playback function
    float process(float sinGain, float squareGain, float sawGain)
    {
        float sinVal = sinSub.process() * sinGain;
        float squareVal = squareSub.process() * squareGain;
        float sawVal = sawSub.process() * sawGain;
        
        float outVal = (sinVal + squareVal + sawVal) * 0.5f;
        
        return outVal;
    }
private:
    // Wavetable instances
    Wavetable sinSub;
    SquareWavetable squareSub;
    SawWavetable sawSub;

};
