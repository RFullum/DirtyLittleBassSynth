/*
  ==============================================================================

    FilterSection.h
    Created: 30 Apr 2020 6:33:55pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

class twoPoleLPF
{
public:
    void setSampleRate(float SR)
    {
        sampleRate = SR;
    }
    
    float processFilter(float freq, float res, float sampleIn)
    {
        cutoffFreq = freq;
        resonance = res;
        inputSample = sampleIn;
        
        return process();
    }
    
private:
    float process()
    {
        lowPass1.reset();
        lowPass2.reset();
        
        lowPass1.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffFreq, resonance) );
        lowPass2.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffFreq, resonance) );
        
        float stage1 = lowPass1.processSingleSampleRaw(inputSample);
        float stage2 = lowPass2.processSingleSampleRaw(stage1);
        
        return stage2;
    }
    
    // IIRFilter instances
    IIRFilter lowPass1;
    IIRFilter lowPass2;
    
    // member variables
    float sampleRate;
    float cutoffFreq;
    float resonance;
    float inputSample;
};


/*
 TO DO:
 -Set up in SynthesiserStarting
 -Test it works
 -Convert to dynamic parameters
 -Add child classes
 */
