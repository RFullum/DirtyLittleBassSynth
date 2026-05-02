/*
  ==============================================================================

    OscillatorParameterControls.cpp
    Created: 26 Dec 2020 12:47:03pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OscillatorParameterControls.h"

//=================================================================

/// blendCurve = 1.0 is linear; > 1.0 reduces blend overlap; 0.1..1.0 increases it.
OscParamControl::OscParamControl()
: blendCurve(0.2f)
{}

float OscParamControl::SinMorphGain(std::atomic<float>* oscMorphVal)
{
    float outVal = LevelFormula(oscMorphVal, 0.0f);
    return pow(outVal, blendCurve);
}

float OscParamControl::SpikeMorphGain(std::atomic<float>* oscMorphVal)
{
    float outVal = LevelFormula(oscMorphVal, 1.0f);
    return pow(outVal, blendCurve);
}

float OscParamControl::SawMorphGain(std::atomic<float>* oscMorphVal)
{
    float outVal = LevelFormula(oscMorphVal, 2.0f);
    return pow(outVal, blendCurve);
}

float OscParamControl::LevelFormula(std::atomic<float>* MV, float CV)
{
    float outVal = 1.0f - fabsf(*MV - CV);

    if (outVal < 0.0f)
        outVal = 0.0f;

    return outVal;
}

//=================================================================

float SubOscParamControl::SinSubGain   (std::atomic<float>* subMorphVal) { return SinMorphGain  (subMorphVal); }
float SubOscParamControl::SquareSubGain(std::atomic<float>* subMorphVal) { return SpikeMorphGain(subMorphVal); }
float SubOscParamControl::SawSubGain   (std::atomic<float>* subMorphVal) { return SawMorphGain  (subMorphVal); }

int SubOscParamControl::SubOctaveSelector(std::atomic<float>* subOctVal)
{
    if (*subOctVal == 1.0f) return 2;
    if (*subOctVal == 2.0f) return 4;
    return 1;
}
