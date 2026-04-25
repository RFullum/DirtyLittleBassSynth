/*
  ==============================================================================

    FilterSection.cpp
    Created: 26 Dec 2020 11:40:49am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "FilterSection.h"

//==============================================================================


TwoPoleLPF::TwoPoleLPF()
: sampleRate(44100.0f)
, maxCutoff(17000.0f)
, minCutoff(20.0f)
, maxResonance(3.0f)
, cutoffFreq(1000.0f)
, resonance(1.0f)
, inputSample(0.0f)
, cutoffScale(0.0f)
, resonanceScale(0.1f)
, resonanceScalePrev(0.1f)
, envelopeVal(0.0f)
, lfoValue(0.0f)
, cutoffLFO(1000.0f)
, cutoffLFOPrev(1000.0f)
{
    lowPass1.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffLFO, resonanceScale));
}

TwoPoleLPF::~TwoPoleLPF() {}

void TwoPoleLPF::SetSampleRate(float SR)
{
    sampleRate = SR;
    lowPass1.reset();
}


float TwoPoleLPF::ProcessFilter(float   noteFreq
                                , float cutoff
                                , float res
                                , float sampleIn
                                , float envVal
                                , float amtToCO
                                , float amtToRes
                                , float lfoVal
                                , float amtToLFO)
{
    KeyMap(noteFreq, cutoff);

    envelopeVal = envVal;
    cutoffSend  = amtToCO;
    resSend     = amtToRes;

    lfoValue = lfoVal;
    lfoSend  = amtToLFO;

    resonance   = res;
    inputSample = sampleIn;

    return Process();
}

void TwoPoleLPF::KeyMap(float frqncy, float CO)
{
    float cutoffPos = CO;
    cutoffFreq      = juce::jmap(cutoffPos, 1.0f, 100.0f, frqncy, maxCutoff);
}

void TwoPoleLPF::FilterEnvControl(float envVal, float amtToCO, float amtToRes)
{
    // Cutoff envelope scaling
    float filterHeadroom = (maxCutoff - cutoffFreq) * amtToCO;
    cutoffScale          = juce::jmap(envVal, 0.0f, 1.0f, cutoffFreq, cutoffFreq + filterHeadroom);
    if (cutoffScale <= 0.0f)
    {
        cutoffScale = 0.01f;
    }

    float resHeadroom = (maxResonance - resonance) * amtToRes;
    float newResScale = juce::jmap(envVal, 0.1f, 0.98f, resonance, resonance + resHeadroom);

    if (resonanceScale != newResScale)
    {
        resonanceScale = newResScale;
        lowPass1.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffLFO, resonanceScale));
    }

    FilterLFOControl();
}

void TwoPoleLPF::FilterLFOControl()
{
    float headroom  = (maxCutoff - cutoffScale) * lfoSend;
    float floorroom = (cutoffScale - minCutoff) * lfoSend;

    if (lfoValue >= 0.0f)
        cutoffLFO = cutoffScale + (lfoValue * headroom);
    else
        cutoffLFO = cutoffScale + (lfoValue * floorroom);
}

float TwoPoleLPF::Process()
{
    FilterEnvControl(envelopeVal, cutoffSend, resSend);
    
    if (cutoffLFOPrev != cutoffLFO
        || resonanceScalePrev != resonanceScale)
    {
        lowPass1.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffLFO, resonanceScale));
        cutoffLFOPrev      = cutoffLFO;
        resonanceScalePrev = resonanceScale;
    }
    
    
    return lowPass1.processSingleSampleRaw(inputSample);
}

//==============================================================================

void FourPoleLPF::SetSampleRate(float SR)
{
    sampleRate = SR;
    twoPole1.SetSampleRate(sampleRate);
    twoPole2.SetSampleRate(sampleRate);
}

float FourPoleLPF::ProcessFilter(float   noteFreq
                                 , float cutoff
                                 , float res
                                 , float sampleIn
                                 , float envVal
                                 , float amtToCO
                                 , float amtToRes
                                 , float lfoVal
                                 , float amtToLFO)
{
    KeyMap(noteFreq, cutoff);

    resonance   = res;
    inputSample = sampleIn;

    float stage1 = twoPole1.ProcessFilter(noteFreq, cutoff, res, sampleIn, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    float stage2 = twoPole2.ProcessFilter(noteFreq, cutoff, res, stage1,   envVal, amtToCO, amtToRes, lfoVal, amtToLFO);

    return stage2;
}

//==============================================================================

void EightPoleLPF::SetSampleRate(float SR)
{
    sampleRate = SR;
    
    fourPole1.SetSampleRate(sampleRate);
    fourPole2.SetSampleRate(sampleRate);
}

float EightPoleLPF::ProcessFilter(float   noteFreq
                                  , float cutoff
                                  , float res
                                  , float sampleIn
                                  , float envVal
                                  , float amtToCO
                                  , float amtToRes
                                  , float lfoVal
                                  , float amtToLFO)
{
    KeyMap(noteFreq, cutoff);

    resonance   = res;
    inputSample = sampleIn;

    float stage1 = fourPole1.ProcessFilter(noteFreq, cutoff, res, sampleIn, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    float stage2 = fourPole2.ProcessFilter(noteFreq, cutoff, res, stage1,   envVal, amtToCO, amtToRes, lfoVal, amtToLFO);

    return stage2;
}

//==============================================================================

void NotchFilter::SetSampleRate(float SR)
{
    sampleRate = SR;
    notchFilter.reset();
}

float NotchFilter::ProcessFilter(float   noteFreq
                                 , float cutoff
                                 , float res
                                 , float sampleIn
                                 , float envVal
                                 , float amtToCO
                                 , float amtToRes
                                 , float lfoVal
                                 , float amtToLFO)
{
    envelopeVal = envVal;
    cutoffSend  = amtToCO;
    resSend     = amtToRes;

    lfoValue = lfoVal;
    lfoSend  = amtToLFO;

    resonance     = res;
    inputSample   = sampleIn;
    float cutFreq = cutoff;
    cutoffFreq    = juce::jmap(cutFreq, 1.0f, 100.0f, 20.0f, maxCutoff);

    return ProcessNotch();
}

float NotchFilter::ProcessNotch()
{
    FilterEnvControl(envelopeVal, cutoffSend, resSend);
    
    if (cutoffLFOPrev != cutoffLFO
        || resonanceScalePrev != resonanceScale)
    {
        notchFilter.setCoefficients(juce::IIRCoefficients::makeNotchFilter(sampleRate, cutoffLFO, resonanceScale));
        
        cutoffLFOPrev      = cutoffLFO;
        resonanceScalePrev = resonanceScale;
    }
    
    return notchFilter.processSingleSampleRaw(inputSample);
}
