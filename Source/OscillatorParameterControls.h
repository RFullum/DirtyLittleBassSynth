/*
  ==============================================================================

    OscillatorParameterControls.h
    Created: 28 Apr 2020 4:17:05pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

class OscParamControl
{
public:
    /**
     Takes the oscillatorMorph value and converts to sinWT gain level:
     0 = 1; 1 = 0; 2 = 0
     */
    float sinMorphGain(std::atomic<float>* oscMorphVal)
    {
        float controlVal = 0.0f;
        float outVal = levelFormula(oscMorphVal, controlVal);
        
        if (outVal < 0.0f)
            outVal = 0.0f;
        
        return outVal;
    }
    
    /**
     Takes the oscillatorMorph value and converts to spikeWT gain level:
     0 = 0; 1 = 1; 2 = 0
     */
    float spikeMorphGain(std::atomic<float>* oscMorphVal)
    {
        float controlVal = 1.0f;
        float outVal = levelFormula(oscMorphVal, controlVal);
        
        return outVal;
    }
    
    float sawMorphGain(std::atomic<float>* oscMorphVal)
    {
        float controlVal = 2.0f;
        float outVal = levelFormula(oscMorphVal, controlVal);
        
        if (outVal < 0.0f)
            outVal = 0.0f;
        
        return outVal;
    }
    
private:
    float levelFormula(std::atomic<float>* MV, float CV)
    {
        float outVal = 1.0f - fabsf(*MV - CV);
        
        return outVal;
    }
    
};
