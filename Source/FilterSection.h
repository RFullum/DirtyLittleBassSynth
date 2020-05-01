/*
  ==============================================================================

    FilterSection.h
    Created: 30 Apr 2020 6:33:55pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

class TwoPoleLPF
{
public:
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        
        lowPass1.reset();
        lowPass2.reset();
        
    }
    
    float processFilter(float noteFreq, std::atomic<float>* cutoff, std::atomic<float>* res, float sampleIn)
    {
        keyMap(noteFreq, cutoff);
        resonance = *res;
        inputSample = sampleIn;
        
        return process();
    }
    
private:
    float process()
    {
        //lowPass1.reset();
        //lowPass2.reset();
        
        lowPass1.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffFreq, resonance) );
        lowPass2.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffFreq, resonance) );
        
        float stage1 = lowPass1.processSingleSampleRaw(inputSample);
        float stage2 = lowPass2.processSingleSampleRaw(stage1);
        
        return stage2;
    }
    
    void keyMap(float frqncy, std::atomic<float>* CO)
    {
        float cutoffPos = *CO;
        cutoffFreq = jmap(cutoffPos, 1.0f, 100.0f, frqncy, 17000.0f);
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
 -Add child classes
 */
