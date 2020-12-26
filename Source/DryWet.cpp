/*
  ==============================================================================

    DryWet.cpp
    Created: 26 Dec 2020 11:16:26am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "DryWet.h"

float DryWet::dryWetMix(float dryIn, float wetIn, float mixVal)
{
    drySample = dryIn;
    wetSample = wetIn;
    mixFactor = mixVal;

    return dryWetCalculation();
}

float DryWet::dryWetCalculation()
{
    float wetScaled = wetSample * mixFactor;
    float dryScaled = drySample * (1.0f - mixFactor);
    
    return dryScaled + wetScaled;
}
