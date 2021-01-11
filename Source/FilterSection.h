/*
  ==============================================================================

    FilterSection.h
    Created: 30 Apr 2020 6:33:55pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/// Creates a -12dB/octave Low Pass Filter by cascading two IIRFilter Low Pass Filters
class TwoPoleLPF
{
public:
    /// Constructor
    TwoPoleLPF();
    
    /// Destructor
    virtual ~TwoPoleLPF();
    
    /// Sets sampleRate of filters and resets
    virtual void setSampleRate(float SR);
    
    /**
     takes current note frequency, cutoff frequency, resonance, input sample value, envelope value, envelop to Cutoff
     amount, and envelope to resonance amount.
     Returns keytracked low pass sample value
     */
    
    virtual float processFilter(float noteFreq, float cutoff,
                                std::atomic<float>* res, float sampleIn, float envVal,
                                std::atomic<float>* amtToCO, std::atomic<float>* amtToRes,
                                float lfoVal, std::atomic<float>* amtToLFO);
    
    void setPlayheadInfo(AudioPlayHead::CurrentPositionInfo& playheadInfo);
    
protected:
    /// Keytracking value mapping
    void keyMap(float frqncy, float CO);
    
    /**
     Scales the ADSR value by the Envelope Amount to Cutoff, that then scales the filter cutoff frequency.
     The current note frequency is the minimum, and the maxCutoff frequency is the maximum.
     */
    void filterEnvControl(float envVal, std::atomic<float>* amtToCO, std::atomic<float>* amtToRes);
    
    /**
     Scales the LFO range, then scales that to the send amount. Modulates filter cutoff,
     clipped to 20Hz and 18000Hz
     */
    void filterLFOControl();
    
    // Member Variables
    float sampleRate;
    float maxCutoff;
    float minCutoff;
    float maxResonance;
    
    float cutoffFreq;
    float resonance;
    float inputSample;
    
    float cutoffScale;
    float resonanceScale;
    
    float envelopeVal;
    float lfoValue;
    float cutoffLFO;
    float hostBPM;
    
    std::atomic<float>* cutoffSend;
    std::atomic<float>* resSend;
    std::atomic<float>* lfoSend;
    
private:
    /// Cascades two IIRFilter lowpasses and returns the output sample value
    float process();

    // IIRFilter instances
    IIRFilter lowPass1;
    
};


//==============================================================================

/// Creates -24dB/octave Lowpass Filter by cascading two TwoPoleLPF class instances
class FourPoleLPF : public TwoPoleLPF
{
public:
    /// Sets sampleRate for filters and resets
    void setSampleRate(float SR) override;
    
    /**
    takes current note frequency, cutoff frequency, resonance, and input sample value.
    Returns keytracked low pass sample value
    */
    float processFilter(float noteFreq, float cutoff,
                        std::atomic<float>* res, float sampleIn, float envVal,
                        std::atomic<float>* amtToCO, std::atomic<float>* amtToRes,
                        float lfoVal, std::atomic<float>* amtToLFO) override;
    
    
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
    void setSampleRate(float SR) override;
    
    /**
    takes current note frequency, cutoff frequency, resonance, and input sample value.
    Returns keytracked low pass sample value
    */
    float processFilter(float noteFreq, float cutoff, std::atomic<float>* res,
                        float sampleIn, float envVal, std::atomic<float>* amtToCO,
                        std::atomic<float>* amtToRes, float lfoVal,
                        std::atomic<float>* amtToLFO) override;
     
    
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
    void setSampleRate(float SR) override;
    
    /**
     Processes Non-keytracked notch filter from 20Hz to 17KHz wth resonance
     */
    float processFilter(float noteFreq, float cutoff,
                        std::atomic<float>* res, float sampleIn, float envVal,
                        std::atomic<float>* amtToCO, std::atomic<float>* amtToRes,
                        float lfoVal, std::atomic<float>* amtToLFO) override;
     
    
private:
    /// Sets notch coeffients and processes inputSample
    float processNotch();
    
    // Instance of IIRFilter class
    IIRFilter notchFilter;
    
};
