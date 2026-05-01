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
OscParamControl::OscParamControl() : blendCurve(0.2f) {}

float OscParamControl::sinMorphGain(std::atomic<float>* oscMorphVal)
{
    float outVal = levelFormula(oscMorphVal, 0.0f);
    return pow(outVal, blendCurve);
}

float OscParamControl::spikeMorphGain(std::atomic<float>* oscMorphVal)
{
    float outVal = levelFormula(oscMorphVal, 1.0f);
    return pow(outVal, blendCurve);
}

float OscParamControl::sawMorphGain(std::atomic<float>* oscMorphVal)
{
    float outVal = levelFormula(oscMorphVal, 2.0f);
    return pow(outVal, blendCurve);
}

float OscParamControl::levelFormula(std::atomic<float>* MV, float CV)
{
    float outVal = 1.0f - fabsf(*MV - CV);

    if (outVal < 0.0f)
        outVal = 0.0f;

    return outVal;
}

//=================================================================

float SubOscParamControl::sinSubGain   (std::atomic<float>* subMorphVal) { return sinMorphGain  (subMorphVal); }
float SubOscParamControl::squareSubGain(std::atomic<float>* subMorphVal) { return spikeMorphGain(subMorphVal); }
float SubOscParamControl::sawSubGain   (std::atomic<float>* subMorphVal) { return sawMorphGain  (subMorphVal); }

int SubOscParamControl::subOctaveSelector(std::atomic<float>* subOctVal)
{
    if (*subOctVal == 1.0f) return 2;
    if (*subOctVal == 2.0f) return 4;
    return 1;
}
