/*
  ==============================================================================

    FilterSectionSV.cpp
    Created: 4 Jan 2021 12:25:32pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "FilterSectionDSP.h"


TwoPoleLPFdsp::TwoPoleLPFdsp()
{
}

TwoPoleLPFdsp::~TwoPoleLPFdsp()
{
}

/// Sets filter with ProcessSpec values in spec: SR = sampleRate, size = maxNumSamples, channels = 1
void TwoPoleLPFdsp::setProcessSpec(float SR, int size)
{
    sampleRate = SR;
    
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = size;
    spec.numChannels      = 1;
    
    //filterSetup();
    lowPassFilterIIR.prepare( spec );
}

/// returns filtered sample: coFreq = cutoff frequency; Q = resonance
float TwoPoleLPFdsp::processFilter(float coFreq, float Q, float inSample)
{
    //float res = *Q;
    
    lpfCoefficients.makeLowPass(sampleRate, coFreq, Q);
    lowPassFilterIIR.coefficients = lpfCoefficients;
    
    float outSample = lowPassFilterIIR.processSample(inSample);
    
    lowPassFilterIIR.snapToZero();
    
    return outSample;
}

/// Sets up filter instance with ProcessSpec spec
void TwoPoleLPFdsp::filterSetup()
{
    lowPassFilter.prepare( spec );
    lowPassFilter.setType( dsp::StateVariableTPTFilterType::lowpass );
    lowPassFilter.reset();
}



