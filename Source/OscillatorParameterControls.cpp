/*
  ==============================================================================

    OscillatorParameterControls.cpp
    Created: 26 Dec 2020 12:47:03pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OscillatorParameterControls.h"

/**
Takes the oscillatorMorph value and converts to sinWT gain level:
value 0 = amplitude 1; val 1 = amp 0; val 2 = amp 0
*/
float OscParamControl::sinMorphGain(std::atomic<float>* oscMorphVal)
{
    float controlVal = 0.0f;
    float outVal = levelFormula(oscMorphVal, controlVal);
    
    if (outVal < 0.0f)
        outVal = 0.0f;
    
    return outVal;
}

/**
Takes the oscillatorMorph value and converts to spikeWT gain level:
val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
*/
float OscParamControl::spikeMorphGain(std::atomic<float>* oscMorphVal)
{
    float controlVal = 1.0f;
    float outVal = levelFormula(oscMorphVal, controlVal);
    
    return outVal;
}

/**
Takes the oscillatorMorph value and converts to sawWT gain level
val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
*/
float OscParamControl::sawMorphGain(std::atomic<float>* oscMorphVal)
{
    float controlVal = 2.0f;
    float outVal = levelFormula(oscMorphVal, controlVal);
    
    if (outVal < 0.0f)
        outVal = 0.0f;
    
    return outVal;
}

/**
Maps parameter value to gain value based on controlVal
Values 0 to 2 :: amplitudes 0 to 1
*/
float OscParamControl::levelFormula(std::atomic<float>* MV, float CV)
{
    float outVal = 1.0f - fabsf(*MV - CV);
    
    return outVal;
}



//=================================================================
//=================================================================

/**
Takes the subOscMorph value and converts to sinSub's gain level:
val 0 = amp 1; val 1 = amp 0; val 2 = amp 0
*/
float SubOscParamControl::sinSubGain(std::atomic<float>* subMorphVal)
{
    return sinMorphGain(subMorphVal);
}

/**
Takes the subOscMorph value and converts to squareSub gain level:
val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
*/
float SubOscParamControl::squareSubGain(std::atomic<float>* subMorphVal)
{
    return spikeMorphGain(subMorphVal);
}

/**
Takes the subOscMorph value and converts to sawSub gain level
val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
*/
float SubOscParamControl::sawSubGain(std::atomic<float>* subMorphVal)
{
    return sawMorphGain(subMorphVal);
}

/**
Converts parameter to denominator for octave frequency: 1, 2, or 4
*/
int SubOscParamControl::subOctaveSelector(std::atomic<float>* subOctVal)
{
    int octave = (int)*subOctVal;
    
    if (octave == 3)
        octave = 4;
    
    return octave;
}
