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

//=======================================================================

class SubOsc
{
public:
    void SetSampleRate(float SR);
    void PopulateWavetable();
    /**
     Sets playback frequency of sub oscillator scaled 0 to 2 octaves below
     noteFreq is the playback note frequency. denom is the denominator to chose sub octave: 1, 2, or 4
     */
    void SetIncrement(float noteFreq, int denom);
    
    /**
     Playback function: gain arguments come from SubOscParamControl class member functions to morph between the three
     wave shapes: sinSubGain( ), squareSubGain( ), and sawSubGain( )
     */
    float Process(float sinGain, float squareGain, float sawGain);
    float GetSquareWavetableValue(int index);
    
    
private:
    Wavetable sinSub;
    SquareWavetable squareSub;
    SawWavetable sawSub;

};
