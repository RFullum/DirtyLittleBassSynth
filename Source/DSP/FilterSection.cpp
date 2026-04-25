/*
  ==============================================================================

    FilterSection.cpp
    Created: 26 Dec 2020 11:40:49am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "FilterSection.h"

//==============================================================================
//==============================================================================

//
// Two Pole LPF Class
//

TwoPoleLPF::TwoPoleLPF() : sampleRate(44100.0f), maxCutoff(17000.0f), minCutoff(20.0f),
                           maxResonance(3.0f), cutoffFreq(1000.0f), resonance(1.0f),
                           inputSample(0.0f), cutoffScale(0.0f), resonanceScale(0.1f),
                           resonanceScalePrev(0.1f), envelopeVal(0.0f), lfoValue(0.0f),
                           cutoffLFO(1000.0f), cutoffLFOPrev(1000.0f)
                           
{
    lowPass1.setCoefficients( IIRCoefficients::makeLowPass(sampleRate, cutoffLFO, resonanceScale) );
}

/// Destructo
TwoPoleLPF::~TwoPoleLPF()
{
}

/// Sets sampleRate of filters and resets
void TwoPoleLPF::setSampleRate(float SR)
{
    sampleRate = SR;

    lowPass1.reset();
}


/**
takes current note frequency, cutoff frequency, resonance, input sample value, envelope value, envelop to Cutoff
amount, and envelope to resonance amount.
Returns keytracked low pass sample value
*/
float TwoPoleLPF::processFilter(float noteFreq, float cutoff,
                            std::atomic<float>* res, float sampleIn, float envVal,
                            std::atomic<float>* amtToCO, std::atomic<float>* amtToRes,
                            float lfoVal, std::atomic<float>* amtToLFO)
{
    keyMap(noteFreq, cutoff);
    
    envelopeVal = envVal;
    cutoffSend  = amtToCO;
    resSend     = amtToRes;
    
    lfoValue = lfoVal;
    lfoSend  = amtToLFO;
    
    resonance   = *res;
    inputSample = sampleIn;
    
    return process();
}


/// Keytracking value mapping
void TwoPoleLPF::keyMap(float frqncy, float CO)
{
    float cutoffPos = CO;
    cutoffFreq      = jmap(cutoffPos, 1.0f, 100.0f, frqncy, maxCutoff);
}

/**
Scales the ADSR value by the Envelope Amount to Cutoff, that then scales the filter cutoff frequency.
The current note frequency is the minimum, and the maxCutoff frequency is the maximum.
*/
void TwoPoleLPF::filterEnvControl(float envVal, std::atomic<float>* amtToCO, std::atomic<float>* amtToRes)
{
    // Cutoff envelope scaling
    float filterHeadroom = (maxCutoff - cutoffFreq) * *amtToCO;
    cutoffScale          = jmap(envVal, 0.0f, 1.0f, cutoffFreq, cutoffFreq + filterHeadroom);
    if (cutoffScale <= 0.0f)
    {
        cutoffScale = 0.01f;
    }
    
    float resHeadroom = (maxResonance - resonance) * *amtToRes;
    float newResScale = jmap(envVal, 0.1f, 0.98f, resonance, resonance + resHeadroom);
    
    if (resonanceScale != newResScale)
    {
        resonanceScale = newResScale;
        lowPass1.setCoefficients ( IIRCoefficients::makeLowPass(sampleRate, cutoffLFO, resonanceScale) );
    }
    
    
    filterLFOControl();
}

/**
Scales the LFO range, then scales that to the send amount. Modulates filter cutoff,
scaled to minCutoff (20Hz) and maxCutoff (17000Hz)
*/
void TwoPoleLPF::filterLFOControl()
{
    
    //float lfo       = lfoValue * *lfoSend;
    float headroom  = (maxCutoff - cutoffScale) * *lfoSend;
    float floorroom = (cutoffScale - minCutoff) * *lfoSend;
    
    if (lfoValue >= 0.0f)
    {
        cutoffLFO = cutoffScale + (lfoValue * headroom);
    }
    else
    {
        cutoffLFO = cutoffScale + (lfoValue * floorroom);
    }
}

/// Cascades two IIRFilter lowpasses and returns the output sample value
float TwoPoleLPF::process()
{
    filterEnvControl         ( envelopeVal, cutoffSend, resSend );
    
    if (cutoffLFOPrev != cutoffLFO || resonanceScalePrev != resonanceScale)
    {
        lowPass1.setCoefficients ( IIRCoefficients::makeLowPass(sampleRate, cutoffLFO, resonanceScale) );
        
        cutoffLFOPrev      = cutoffLFO;
        resonanceScalePrev = resonanceScale;
    }
    
    
    return lowPass1.processSingleSampleRaw(inputSample);
}

/*
void TwoPoleLPF::setPlayheadInfo(AudioPlayHead::CurrentPositionInfo& playheadInfo)
{
    if (hostBPM != (float)playheadInfo.bpm)
    {
        hostBPM = playheadInfo.bpm;
    }
}
*/

//==============================================================================
//==============================================================================

//
// Four Pole LPF Class
//

/// Sets sampleRate for filters and resets 
void FourPoleLPF::setSampleRate(float SR)
{
    sampleRate = SR;
    
    twoPole1.setSampleRate(sampleRate);
    twoPole2.setSampleRate(sampleRate);
}


/**
takes current note frequency, cutoff frequency, resonance, and input sample value.
Returns keytracked low pass sample value
*/
float FourPoleLPF::processFilter(float noteFreq, float cutoff,
                                 std::atomic<float>* res, float sampleIn, float envVal,
                                 std::atomic<float>* amtToCO, std::atomic<float>* amtToRes,
                                 float lfoVal, std::atomic<float>* amtToLFO)
{
    keyMap(noteFreq, cutoff);
    
    resonance   = *res;
    inputSample = sampleIn;
    
    float stage1 = twoPole1.processFilter(noteFreq, cutoff, res, sampleIn, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    float stage2 = twoPole2.processFilter(noteFreq, cutoff, res, stage1, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    
    return stage2;
}


//==============================================================================
//==============================================================================

//
// Eight Pole LPF
//

/// Sets sampleRate for filters and resets
void EightPoleLPF::setSampleRate(float SR)
{
    sampleRate = SR;
    
    fourPole1.setSampleRate(sampleRate);
    fourPole2.setSampleRate(sampleRate);
}


/**
takes current note frequency, cutoff frequency, resonance, and input sample value.
Returns keytracked low pass sample value
*/
float EightPoleLPF::processFilter(float noteFreq, float cutoff, std::atomic<float>* res,
                    float sampleIn, float envVal, std::atomic<float>* amtToCO,
                    std::atomic<float>* amtToRes, float lfoVal,
                    std::atomic<float>* amtToLFO)
{
    keyMap(noteFreq, cutoff);
    
    resonance   = *res;
    inputSample = sampleIn;
    
    float stage1 = fourPole1.processFilter(noteFreq, cutoff, res, sampleIn, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    float stage2 = fourPole2.processFilter(noteFreq, cutoff, res, stage1, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    
    return stage2;
}

//==============================================================================
//==============================================================================

//
// Notch Filter Class
//

/// Sets sampleRate of filters and resets
void NotchFilter::setSampleRate(float SR)
{
    sampleRate = SR;
    
    notchFilter.reset();
}


/**
Processes Non-keytracked notch filter from 20Hz to 17KHz wth resonance
*/
float NotchFilter::processFilter(float noteFreq, float cutoff,
                                 std::atomic<float>* res, float sampleIn, float envVal,
                                 std::atomic<float>* amtToCO, std::atomic<float>* amtToRes,
                                 float lfoVal, std::atomic<float>* amtToLFO)
{
    envelopeVal = envVal;
    cutoffSend  = amtToCO;
    resSend     = amtToRes;
    
    lfoValue = lfoVal;
    lfoSend  = amtToLFO;
    
    resonance     = *res;
    inputSample   = sampleIn;
    float cutFreq = cutoff;
    cutoffFreq    = jmap(cutFreq, 1.0f, 100.0f, 20.0f, maxCutoff);
    
    return processNotch();
}


/// Sets notch coeffients and processes inputSample
float NotchFilter::processNotch()
{
    filterEnvControl            ( envelopeVal, cutoffSend, resSend );
    
    if (cutoffLFOPrev != cutoffLFO || resonanceScalePrev != resonanceScale)
    {
        notchFilter.setCoefficients ( IIRCoefficients::makeNotchFilter(sampleRate, cutoffLFO, resonanceScale) );
        
        cutoffLFOPrev      = cutoffLFO;
        resonanceScalePrev = resonanceScale;
    }
    //notchFilter.setCoefficients ( IIRCoefficients::makeNotchFilter(sampleRate, cutoffLFO, resonanceScale) );
    
    return notchFilter.processSingleSampleRaw(inputSample);
}
