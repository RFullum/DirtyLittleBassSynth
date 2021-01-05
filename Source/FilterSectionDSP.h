/*
  ==============================================================================

    FilterSectionSV.h
    Created: 4 Jan 2021 12:25:32pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class TwoPoleLPFdsp
{
public:
    TwoPoleLPFdsp();
    ~TwoPoleLPFdsp();
    
    /// Sets ProcessSpec values in spec: SR = sampleRate, size = maxNumSamples, channels = 1
    void setProcessSpec(float SR, int size);
    
    float processFilter(float coFreq, float Q, float inSample);
    
    
private:
    
    void filterSetup();
    
    dsp::StateVariableTPTFilter<float> lowPassFilter;
    
    dsp::ProcessSpec spec;
    
    dsp::IIR::Coefficients<float> lpfCoefficients;
    dsp::IIR::Filter<float> lowPassFilterIIR;
    
    float sampleRate;
};
