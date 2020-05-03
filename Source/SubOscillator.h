/*
  ==============================================================================

    SubOscillator.h
    Created: 28 Apr 2020 2:17:12pm
    Author:  B150987

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
    
    /**
     Sets playback frequency of sub oscillator scaled 0 to 2 octaves below
     noteFreq is the playback note frequency. denom is the denominator to chose sub octave: 1, 2, or 4
     */
    void setIncrement(float noteFreq, int denom)
    {
        float subFreq = noteFreq / (float)denom;
        
        sinSub.setIncrement(subFreq);
        squareSub.setIncrement(subFreq);
        sawSub.setIncrement(subFreq);
    }
    
    /// Playback function
    /**
     Playback function: gain arguments come from SubOscParamControl class member functions to morph between the three
     wave shapes: sinSubGain( ), squareSubGain( ), and sawSubGain( )
     */
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
