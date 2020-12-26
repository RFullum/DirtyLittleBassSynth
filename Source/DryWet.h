/*
  ==============================================================================

    DryWet.h
    Created: 29 Apr 2020 10:13:04pm
    Author:  B150987

  ==============================================================================
*/

#pragma once


/// Dry/Wet Control
class DryWet
{
public:
    /**
     Takes the dry sample value, wet sample value, and mix value. Returns their mix sample value
     */
    float dryWetMix(float dryIn, float wetIn, float mixVal);
    
    
private:
    /// Calculates gain of dry and wet samples. Returns mix sample
    float dryWetCalculation();
    

    // member variables
    float drySample;
    float wetSample;
    float mixFactor;
};
