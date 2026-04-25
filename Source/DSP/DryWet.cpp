/*
  ==============================================================================

    DryWet.cpp
    Created: 26 Dec 2020 11:16:26am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "DryWet.h"

DryWet::DryWet() : drySample(0.0f), wetSample(0.0f), mixFactor(0.0f)
{
}

/**
Takes the dry sample value, wet sample value, and mix value. Returns their mix sample value
*/
float DryWet::dryWetMix(float dryIn, float wetIn, float mixVal)
{
    drySample = dryIn;
    wetSample = wetIn;
    mixFactor = mixVal;

    return dryWetCalculation();
}

/// Calculates gain of dry and wet samples. Returns mix sample
float DryWet::dryWetCalculation()
{
    float wetScaled = wetSample * mixFactor;
    float dryScaled = drySample * (1.0f - mixFactor);
    
    return dryScaled + wetScaled;
}
