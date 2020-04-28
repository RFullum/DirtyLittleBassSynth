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
    
    /**
     Takes the oscillatorMorph value and converts to sawWT gain level
     0 = 0; 1 = 0; 2 = 1
     */
    float sawMorphGain(std::atomic<float>* oscMorphVal)
    {
        float controlVal = 2.0f;
        float outVal = levelFormula(oscMorphVal, controlVal);
        
        if (outVal < 0.0f)
            outVal = 0.0f;
        
        return outVal;
    }
    
private:
    /**
     Maps parameter value to gain value
     0 to 2 :: 0 to 1
     */
    float levelFormula(std::atomic<float>* MV, float CV)
    {
        float outVal = 1.0f - fabsf(*MV - CV);
        
        return outVal;
    }
    
};



//=================================================================

class SubOscParamControl : public OscParamControl
{
public:
    /**
    Takes the subOscMorph value and converts to sinSub's gain level:
    0 = 1; 1 = 0; 2 = 0
    */
    float sinSubGain(std::atomic<float>* subMorphVal)
    {
        return sinMorphGain(subMorphVal);
    }
    
    /**
    Takes the subOscMorph value and converts to squareSub gain level:
    0 = 0; 1 = 1; 2 = 0
    */
    float squareSubGain(std::atomic<float>* subMorphVal)
    {
        return spikeMorphGain(subMorphVal);
    }
    
    /**
    Takes the subOscMorph value and converts to sawSub gain level
    0 = 0; 1 = 0; 2 = 1
    */
    float sawSubGain(std::atomic<float>* subMorphVal)
    {
        return sawMorphGain(subMorphVal);
    }
};
