/*
  ==============================================================================

    DryWet.cpp
    Created: 26 Dec 2020 11:16:26am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "DryWet.h"

DryWet::DryWet()
: drySample(0.0f)
, wetSample(0.0f)
, mixFactor(0.0f)
{}

DryWet::~DryWet() {}

float DryWet::DryWetMix(float dryIn, float wetIn, float mixVal)
{
    drySample = dryIn;
    wetSample = wetIn;
    mixFactor = mixVal;

    return DryWetCalculation();
}

float DryWet::DryWetCalculation()
{
    float wetScaled = wetSample * mixFactor;
    float dryScaled = drySample * (1.0f - mixFactor);
    
    return dryScaled + wetScaled;
}
