/*
  ==============================================================================

    Modifiers.cpp
    Created: 26 Dec 2020 12:19:57pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Modifiers.h"

//==============================================================================

RingMod::RingMod()
: sampleRate(44100.0f)
, modFrequency(0.0f)
, ringToneSlider(0.0f)
{}

RingMod::~RingMod(){}

void RingMod::SetSampleRate(float SR)
{
    sampleRate = SR;
    SetUpWavetables();
}

void RingMod::ModFreq(float fqncy, std::atomic<float> *offset)
{
    modFrequency = fqncy * *offset;
    SetRingIncrement();
}

void RingMod::SetRingToneSlider(std::atomic<float> *toneSlider)
{
    ringToneSlider = *toneSlider;
}

float RingMod::Process()
{
    return RingModProcess();
}

void RingMod::SetUpWavetables()
{
    wtSine.SetSampleRate  (sampleRate);
    wtSquare.SetSampleRate(sampleRate);
    
    wtSine.PopulateWavetable();
    wtSquare.PopulateWavetable();
}

void RingMod::SetRingIncrement()
{
    wtSine.SetIncrement  (modFrequency);
    wtSquare.SetIncrement(modFrequency);
}

float RingMod::RingModProcess()
{
    float sinVal = wtSine.Process();
    float sqVal  = wtSquare.Process();
    float outVal = toneControl.DryWetMix(sinVal, sqVal, ringToneSlider);
    
    return outVal;
}

//==============================================================================

FrequencyShifter::FrequencyShifter()
: sineLevel(1.0f)
, spikeLevel(0.0f)
, sawLevel(0.0f)
{}

void FrequencyShifter::SetSampleRate(float SR)
{
    sampleRate = SR;
    SetUpFreqShiftWavetables();
}

void FrequencyShifter::ModFreq(float fqncy, std::atomic<float> *offset)
{
    modFrequency = fqncy * *offset;
    SetFreqShiftIncrement();
}

void FrequencyShifter::OscMorph(std::atomic<float> *morph)
{
    sineLevel  = oscParamControl.sinMorphGain(morph);
    spikeLevel = oscParamControl.spikeMorphGain(morph);
    sawLevel   = oscParamControl.sawMorphGain(morph);
}

float FrequencyShifter::Process()
{
    return FreqShiftProcess();
}

void FrequencyShifter::SetUpFreqShiftWavetables()
{
    wtSine.SetSampleRate (sampleRate);
    wtSaw.SetSampleRate  (sampleRate);
    wtSpike.SetSampleRate(sampleRate);
    
    wtSine.PopulateWavetable();
    wtSaw.PopulateWavetable();
    wtSpike.PopulateWavetable();
}

void FrequencyShifter::SetFreqShiftIncrement()
{
    wtSine.SetIncrement (modFrequency);
    wtSaw.SetIncrement  (modFrequency);
    wtSpike.SetIncrement(modFrequency);
}

float FrequencyShifter::FreqShiftProcess()
{
    float sinVal   = wtSine .Process() * sineLevel;
    float spikeVal = wtSpike.Process() * spikeLevel;
    float sawVal   = wtSaw  .Process() * sawLevel;
    
    return (sinVal + spikeVal + sawVal) * 0.5f;
}

//==============================================================================

SampleAndHold::SampleAndHold()
: oscSampleVal(0.0f)
, holdSampleVal(0.0f)
{}

void SampleAndHold::SetSampleRate(float SR)
{
    sampleRate = SR;
    SetSampHoldWavetables();
}

void SampleAndHold::ModFreq(float fqncy, std::atomic<float> *offset)
{
    modFrequency = fqncy * *offset;
    SetSampHoldIncrement();
}

float SampleAndHold::ProcessSH(float oscSampleValIn)
{
    oscSampleVal = oscSampleValIn;
    return SampleHoldProcess();
}

void SampleAndHold::SetSampHoldWavetables()
{
    wtSampHold.SetSampleRate(sampleRate);
    wtSampHold.PopulateWavetable();
}

void SampleAndHold::SetSampHoldIncrement()
{
    wtSampHold.SetIncrement(modFrequency);
}

float SampleAndHold::SampleHoldProcess()
{
    float outVal;
    float sampHoldVal = wtSampHold.Process();
    
    if (sampHoldVal >= 0.0f)
    {
        outVal        = oscSampleVal;
        holdSampleVal = outVal;
    }
    else
    {
        outVal = holdSampleVal;
    }
        
    return outVal;
}
