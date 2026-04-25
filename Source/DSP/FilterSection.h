/*
  ==============================================================================

    FilterSection.h
    Created: 30 Apr 2020 6:33:55pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class TwoPoleLPF
{
public:
    TwoPoleLPF();
    virtual ~TwoPoleLPF();

    virtual void  SetSampleRate(float SR);
    virtual float ProcessFilter(float   noteFreq
                                , float cutoff
                                , float res
                                , float sampleIn
                                , float envVal
                                , float amtToCO
                                , float amtToRes
                                , float lfoVal
                                , float amtToLFO);

protected:
    void KeyMap(float frqncy, float CO);
    void FilterEnvControl(float envVal, float amtToCO, float amtToRes);
    void FilterLFOControl();

    float sampleRate;
    float maxCutoff;
    float minCutoff;
    float maxResonance;

    float cutoffFreq;
    float resonance;
    float inputSample;

    float cutoffScale;
    float resonanceScale;
    float resonanceScalePrev;

    float envelopeVal;
    float lfoValue;
    float cutoffLFO;
    float cutoffLFOPrev;

    float cutoffSend;
    float resSend;
    float lfoSend;

private:
    float Process();

    juce::IIRFilter lowPass1;
};


//==============================================================================

class FourPoleLPF
    : public TwoPoleLPF
{
public:
    void  SetSampleRate(float SR) override;
    float ProcessFilter(float   noteFreq
                        , float cutoff
                        , float res
                        , float sampleIn
                        , float envVal
                        , float amtToCO
                        , float amtToRes
                        , float lfoVal
                        , float amtToLFO) override;

private:
    TwoPoleLPF twoPole1;
    TwoPoleLPF twoPole2;
};


//==============================================================================

class EightPoleLPF
    : public FourPoleLPF
{
public:
    void  SetSampleRate(float SR) override;
    float ProcessFilter(float   noteFreq
                        , float cutoff
                        , float res
                        , float sampleIn
                        , float envVal
                        , float amtToCO
                        , float amtToRes
                        , float lfoVal
                        , float amtToLFO) override;

private:
    FourPoleLPF fourPole1;
    FourPoleLPF fourPole2;
};

//==============================================================================

class NotchFilter
    : public TwoPoleLPF
{
public:
    void  SetSampleRate(float SR) override;
    float ProcessFilter(float   noteFreq
                        , float cutoff
                        , float res
                        , float sampleIn
                        , float envVal
                        , float amtToCO
                        , float amtToRes
                        , float lfoVal
                        , float amtToLFO) override;

private:
    float ProcessNotch();

    juce::IIRFilter notchFilter;
};
