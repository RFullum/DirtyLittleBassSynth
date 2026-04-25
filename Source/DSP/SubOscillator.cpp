/*
  ==============================================================================

    SubOscillator.cpp
    Created: 26 Dec 2020 3:16:45pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "SubOscillator.h"

/// Sets sampleRate of sub oscillators
void SubOsc::setSampleRate(float SR)
{
    sinSub.SetSampleRate    (SR);
    squareSub.SetSampleRate (SR);
    sawSub.SetSampleRate    (SR);
}


/// Populates values to the sub oscillator wavetables
void SubOsc::populateWavetable()
{
    sinSub.PopulateWavetable();
    squareSub.PopulateWavetable();
    sawSub.PopulateWavetable();
}


/**
Sets playback frequency of sub oscillator scaled 0 to 2 octaves below
noteFreq is the playback note frequency. denom is the denominator to chose sub octave: 1, 2, or 4
*/
void SubOsc::setIncrement(float noteFreq, int denom)
{
    float subFreq = noteFreq / (float)denom;
    
    sinSub.SetIncrement    (subFreq);
    squareSub.SetIncrement (subFreq);
    sawSub.SetIncrement    (subFreq);
}


/// Returns the sample value of the wavetable at the index
float SubOsc::getSquareWavetableValue(int index)
{
    return squareSub.GetWavetableSampleValue(index);
}

/**
Playback function: gain arguments come from SubOscParamControl class member functions to morph between the three
wave shapes: sinSubGain( ), squareSubGain( ), and sawSubGain( )
*/
float SubOsc::process(float sinGain, float squareGain, float sawGain)
{
    float sinVal    = sinSub   .Process() * sinGain;
    float squareVal = squareSub.Process() * squareGain;
    float sawVal    = sawSub   .Process() * sawGain;
    
    float outVal = (sinVal + squareVal + sawVal) * 0.5f;
    
    return outVal;
}
