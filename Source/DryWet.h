/*
  ==============================================================================

    DryWet.h
    Created: 29 Apr 2020 10:13:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

class DryWet
{
public:
    float dryWetMix(float dryIn, float wetIn, std::atomic<float>* mixVal)
    {
        drySample = dryIn;
        wetSample = wetIn;
        mixFactor = *mixVal;

        return dryWetCalculation();
    }
    
private:
    float dryWetCalculation()
    {
        float wetScaled = wetSample * mixFactor;
        float dryScaled = drySample * (1.0f - mixFactor);
        
        return dryScaled + wetScaled;
    }
    
    // member variables
    float drySample;
    float wetSample;
    float mixFactor;
};
