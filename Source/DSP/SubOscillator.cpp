/*
  ==============================================================================

    SubOscillator.cpp
    Created: 26 Dec 2020 3:16:45pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "SubOscillator.h"

//=======================================================================

void SubOsc::SetSampleRate(float SR)
{
    sinSub.SetSampleRate   (SR);
    squareSub.SetSampleRate(SR);
    sawSub.SetSampleRate   (SR);
}

void SubOsc::PopulateWavetable()
{
    sinSub.PopulateWavetable();
    squareSub.PopulateWavetable();
    sawSub.PopulateWavetable();
}

void SubOsc::SetIncrement(float noteFreq, int denom)
{
    float subFreq = noteFreq / (float)denom;
    
    sinSub.SetIncrement   (subFreq);
    squareSub.SetIncrement(subFreq);
    sawSub.SetIncrement   (subFreq);
}

float SubOsc::Process(float sinGain, float squareGain, float sawGain)
{
    float sinVal    = sinSub   .Process() * sinGain;
    float squareVal = squareSub.Process() * squareGain;
    float sawVal    = sawSub   .Process() * sawGain;

    float outVal = (sinVal + squareVal + sawVal) * 0.5f;

    return outVal;
}

float SubOsc::GetSquareWavetableValue(int index)
{
    return squareSub.GetWavetableSampleValue(index);
}
