/*
  ==============================================================================

    FilterSection.cpp
    Created: 26 Dec 2020 11:40:49am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "FilterSection.h"

//==============================================================================

void FilterModulation::KeyMapTracked(float noteFreq, float cutoffPos)
{
    // Floor sits three octaves below the played note Exponential map → uniform octave
    // coverage per slider unit.
    const float floor = juce::jmax(minCutoff, noteFreq * 0.125f);
    const float t     = juce::jlimit(0.0f, 1.0f, cutoffPos);
    cutoffFreq        = floor * std::pow(maxCutoff / floor, t);
}

void FilterModulation::KeyMapFixed(float cutoffPos)
{
    const float t = juce::jlimit(0.0f, 1.0f, cutoffPos);
    cutoffFreq    = minCutoff * std::pow(maxCutoff / minCutoff, t);
}

void FilterModulation::ApplyEnvAndLfo(float envVal, float amtToCO, float amtToRes, float lfoVal, float amtToLFO)
{
    // Cutoff envelope scaling
    float filterHeadroom = (maxCutoff - cutoffFreq) * amtToCO;
    float cutoffScale    = juce::jmap(envVal, 0.0f, 1.0f, cutoffFreq, cutoffFreq + filterHeadroom);
    if (cutoffScale <= 0.0f)
        cutoffScale = 0.01f;

    // Resonance envelope scaling
    float resHeadroom = (maxResonance - resonance) * amtToRes;
    resonanceScale    = juce::jmap(envVal, 0.1f, 0.98f, resonance, resonance + resHeadroom);

    // LFO modulation of cutoff (asymmetric headroom above / floor below)
    float headroom  = (maxCutoff - cutoffScale) * amtToLFO;
    float floorroom = (cutoffScale - minCutoff) * amtToLFO;

    if (lfoVal >= 0.0f)
        cutoffLFO = cutoffScale + (lfoVal * headroom);
    else
        cutoffLFO = cutoffScale + (lfoVal * floorroom);
}

bool FilterModulation::CoefficientsNeedUpdate() const
{
    return cutoffLFOPrev != cutoffLFO || resonanceScalePrev != resonanceScale;
}

void FilterModulation::AcknowledgeUpdate()
{
    cutoffLFOPrev      = cutoffLFO;
    resonanceScalePrev = resonanceScale;
}

//==============================================================================

TwoPoleLPF::TwoPoleLPF()
{
    lowPass.setCoefficients(juce::IIRCoefficients::makeLowPass(mod.sampleRate, mod.cutoffLFO, mod.resonanceScale));
}

void TwoPoleLPF::SetSampleRate(float SR)
{
    mod.sampleRate = SR;
    lowPass.reset();
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
    mod.KeyMapTracked(noteFreq, cutoff);
    mod.resonance = res;
    mod.ApplyEnvAndLfo(envVal, amtToCO, amtToRes, lfoVal, amtToLFO);

    if (mod.CoefficientsNeedUpdate())
    {
        lowPass.setCoefficients(juce::IIRCoefficients::makeLowPass(mod.sampleRate, mod.cutoffLFO, mod.resonanceScale));
        mod.AcknowledgeUpdate();
    }

    return lowPass.processSingleSampleRaw(sampleIn);
}

//==============================================================================

void FourPoleLPF::SetSampleRate(float SR)
{
    stage1.SetSampleRate(SR);
    stage2.SetSampleRate(SR);
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
    float s1 = stage1.ProcessFilter(noteFreq, cutoff, res, sampleIn, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    float s2 = stage2.ProcessFilter(noteFreq, cutoff, res, s1,       envVal, amtToCO, amtToRes, lfoVal, amtToLFO);

    return s2;
}

//==============================================================================

void EightPoleLPF::SetSampleRate(float SR)
{
    stage1.SetSampleRate(SR);
    stage2.SetSampleRate(SR);
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
    float s1 = stage1.ProcessFilter(noteFreq, cutoff, res, sampleIn, envVal, amtToCO, amtToRes, lfoVal, amtToLFO);
    float s2 = stage2.ProcessFilter(noteFreq, cutoff, res, s1,       envVal, amtToCO, amtToRes, lfoVal, amtToLFO);

    return s2;
}

//==============================================================================

void NotchFilter::SetSampleRate(float SR)
{
    mod.sampleRate = SR;
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
    mod.KeyMapTracked(noteFreq, cutoff);
    mod.resonance = res;
    mod.ApplyEnvAndLfo(envVal, amtToCO, amtToRes, lfoVal, amtToLFO);

    if (mod.CoefficientsNeedUpdate())
    {
        notchFilter.setCoefficients(juce::IIRCoefficients::makeNotchFilter(mod.sampleRate, mod.cutoffLFO, mod.resonanceScale));
        mod.AcknowledgeUpdate();
    }

    return notchFilter.processSingleSampleRaw(sampleIn);
}
