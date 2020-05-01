/*
  ==============================================================================

    FilterSection.h
    Created: 30 Apr 2020 6:33:55pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

/// Creates a -12dB/octave Low Pass Filter by cascading two IIRFilter Low Pass Filters
class TwoPoleLPF
{
public:
    /// Destructor
    virtual ~TwoPoleLPF()
    {
        
    }
    
    /// Sets sampleRate of filters and resets
    virtual void setSampleRate(float SR)
    {
        sampleRate = SR;
        
        lowPass1.reset();
        lowPass2.reset();
    }
    
    /**
     takes current note frequency, cutoff frequency, resonance, and input sample value.
     Returns keytracked low pass sample value
     */
    virtual float processFilter(float noteFreq, std::atomic<float>* cutoff, std::atomic<float>* res, float sampleIn)
    {
        keyMap(noteFreq, cutoff);
        resonance = *res;
        inputSample = sampleIn;
        
        return process(); // noteFreq, cutoff
    }
    
protected:
    /// Keytracking value mapping
    void keyMap(float frqncy, std::atomic<float>* CO)
    {
        float cutoffPos = *CO;
        cutoffFreq = jmap(cutoffPos, 1.0f, 100.0f, frqncy, 17000.0f);
    }
    
    // Member Variables
    float sampleRate;
    float cutoffFreq;
    float resonance;
    float inputSample;
    
private:
    /// Cascades two IIRFilter lowpasses and returns the output sample value
    float process() // float frqncy, std::atomic<float>* cutoffAmt
    {
        lowPass1.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffFreq, resonance) );
        lowPass2.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffFreq, resonance) );
        
        float stage1 = lowPass1.processSingleSampleRaw(inputSample);
        float stage2 = lowPass2.processSingleSampleRaw(stage1);
        
        return stage2;
    }
    // IIRFilter instances
    IIRFilter lowPass1;
    IIRFilter lowPass2;
    
};


//==============================================================================

/// Creates -24dB/octave Lowpass Filter by cascading two TwoPoleLPF class instances
class FourPoleLPF : public TwoPoleLPF
{
public:
    /// Sets sampleRate for filters and resets
    void setSampleRate(float SR) override
    {
        sampleRate = SR;
        
        twoPole1.setSampleRate(sampleRate);
        twoPole2.setSampleRate(sampleRate);
    }
    
    /**
    takes current note frequency, cutoff frequency, resonance, and input sample value.
    Returns keytracked low pass sample value
    */
    float processFilter(float noteFreq, std::atomic<float>* cutoff, std::atomic<float>* res, float sampleIn) override
    {
        keyMap(noteFreq, cutoff);
        resonance = *res;
        inputSample = sampleIn;
        
        float stage1 = twoPole1.processFilter(noteFreq, cutoff, res, sampleIn);
        float stage2 = twoPole2.processFilter(noteFreq, cutoff, res, stage1);
        
        return stage2;
    }
    
private:
    // Instances of TwoPoleLPF classes
    TwoPoleLPF twoPole1;
    TwoPoleLPF twoPole2;
};


//==============================================================================


/// Creates -48dB/octave Lowpass Filter by cascading two FourPoleLPF classes
class EightPoleLPF : public FourPoleLPF
{
public:
    /// Sets sampleRate for filters and resets
    void setSampleRate(float SR) override
    {
        sampleRate = SR;
        
        fourPole1.setSampleRate(sampleRate);
        fourPole2.setSampleRate(sampleRate);
    }
    
    /**
    takes current note frequency, cutoff frequency, resonance, and input sample value.
    Returns keytracked low pass sample value
    */
    float processFilter(float noteFreq, std::atomic<float>* cutoff, std::atomic<float>* res, float sampleIn) override
    {
        keyMap(noteFreq, cutoff);
        resonance = *res;
        inputSample = sampleIn;
        
        float stage1 = fourPole1.processFilter(noteFreq, cutoff, res, sampleIn);
        float stage2 = fourPole2.processFilter(noteFreq, cutoff, res, stage1);
        
        return stage2;
    }
    
private:
    // Instances of FourPoleLPF classes
    FourPoleLPF fourPole1;
    FourPoleLPF fourPole2;
};




//==============================================================================


class NotchFilter : public TwoPoleLPF
{
public:
    /// Sets sampleRate of filters and resets
    void setSampleRate(float SR) override
    {
        sampleRate = SR;
        
        notchFilter.reset();
    }
    
    /**
     Processes Non-keytracked notch filter from 20Hz to 17KHz wth resonance
     */
    float processFilter(float noteFreq, std::atomic<float>* cutoff, std::atomic<float>* res, float sampleIn) override
    {
        resonance = *res;
        inputSample = sampleIn;
        float cutFreq = *cutoff;
        cutoffFreq = jmap(cutFreq, 1.0f, 100.0f, 20.0f, 17000.0f);
        
        return processNotch();
    }
private:
    /// Sets notch coeffients and processes inputSample
    float processNotch()
    {
        notchFilter.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffFreq, resonance) );
        
        return notchFilter.processSingleSampleRaw(inputSample);
    }
    
    // Instance of IIRFilter class
    IIRFilter notchFilter;
    
};
