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
    sinSub.setSampleRate(SR);
    squareSub.setSampleRate(SR);
    sawSub.setSampleRate(SR);
}

/// Populates values to the sub oscillator wavetables
void SubOsc::populateWavetable()
{
    sinSub.populateWavetable();
    squareSub.populateWavetable();
    sawSub.populateWavetable();
}

/**
Sets playback frequency of sub oscillator scaled 0 to 2 octaves below
noteFreq is the playback note frequency. denom is the denominator to chose sub octave: 1, 2, or 4
*/
void SubOsc::setIncrement(float noteFreq, int denom)
{
    float subFreq = noteFreq / (float)denom;
    
    sinSub.setIncrement    (subFreq);
    squareSub.setIncrement (subFreq);
    sawSub.setIncrement    (subFreq);
}

float SubOsc::getSquareWavetableValue(int index)
{
    return squareSub.getWavetableSampleValue(index);
}

/**
Playback function: gain arguments come from SubOscParamControl class member functions to morph between the three
wave shapes: sinSubGain( ), squareSubGain( ), and sawSubGain( )
*/
float SubOsc::process(float sinGain, float squareGain, float sawGain)
{
    float sinVal    = sinSub.process() * sinGain;
    float squareVal = squareSub.process() * squareGain;
    float sawVal    = sawSub.process() * sawGain;
    
    float outVal = (sinVal + squareVal + sawVal) * 0.5f;
    
    return outVal;
}
