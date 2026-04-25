/*
  ==============================================================================

    DryWet.h
    Created: 29 Apr 2020 10:13:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

//============================================================

class DryWet
{
public:
    DryWet();
    ~DryWet();
    
    float DryWetMix(float dryIn, float wetIn, float mixVal);
    
private:
    float DryWetCalculation();

    float drySample;
    float wetSample;
    float mixFactor;
};
