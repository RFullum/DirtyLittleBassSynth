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

void RingMod::SetSampleRate(float SR)
{
    sampleRate = SR;
    SetUpWavetables();
}

void RingMod::ModFreq(float fqncy, float offset)
{
    modFrequency = fqncy * offset;
    SetIncrement();
}

void RingMod::SetRingToneSlider(float toneSlider)
{
    ringToneSlider = toneSlider;
}

float RingMod::Process()
{
    float sinVal = wtSine  .Process();
    float sqVal  = wtSquare.Process();

    return DryWetMix(sinVal, sqVal, ringToneSlider);
}

void RingMod::SetUpWavetables()
{
    wtSine  .SetSampleRate(sampleRate);
    wtSquare.SetSampleRate(sampleRate);

    wtSine  .PopulateWavetable();
    wtSquare.PopulateWavetable();
}

void RingMod::SetIncrement()
{
    wtSine  .SetIncrement(modFrequency);
    wtSquare.SetIncrement(modFrequency);
}

//==============================================================================

FrequencyShifter::FrequencyShifter()
: sampleRate(44100.0f)
, modFrequency(0.0f)
, sineLevel(1.0f)
, spikeLevel(0.0f)
, sawLevel(0.0f)
{}

void FrequencyShifter::SetSampleRate(float SR)
{
    sampleRate = SR;
    SetUpWavetables();
}

void FrequencyShifter::ModFreq(float fqncy, float offset)
{
    modFrequency = fqncy * offset;
    SetIncrement();
}

void FrequencyShifter::OscMorph(float sinLevelIn, float spikeLevelIn, float sawLevelIn)
{
    sineLevel  = sinLevelIn;
    spikeLevel = spikeLevelIn;
    sawLevel   = sawLevelIn;
}

float FrequencyShifter::Process()
{
    float sinVal   = wtSine .Process() * sineLevel;
    float spikeVal = wtSpike.Process() * spikeLevel;
    float sawVal   = wtSaw  .Process() * sawLevel;

    return (sinVal + spikeVal + sawVal) * 0.5f;
}

void FrequencyShifter::SetUpWavetables()
{
    wtSine .SetSampleRate(sampleRate);
    wtSaw  .SetSampleRate(sampleRate);
    wtSpike.SetSampleRate(sampleRate);

    wtSine .PopulateWavetable();
    wtSaw  .PopulateWavetable();
    wtSpike.PopulateWavetable();
}

void FrequencyShifter::SetIncrement()
{
    wtSine .SetIncrement(modFrequency);
    wtSaw  .SetIncrement(modFrequency);
    wtSpike.SetIncrement(modFrequency);
}

//==============================================================================

SampleAndHold::SampleAndHold()
: sampleRate(44100.0f)
, modFrequency(0.0f)
, oscSampleVal(0.0f)
, holdSampleVal(0.0f)
{}

void SampleAndHold::SetSampleRate(float SR)
{
    sampleRate = SR;
    SetUpWavetable();
}

void SampleAndHold::ModFreq(float fqncy, float offset)
{
    modFrequency = fqncy * offset;
    SetIncrement();
}

float SampleAndHold::ProcessSH(float oscSampleValIn)
{
    oscSampleVal = oscSampleValIn;

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

void SampleAndHold::SetUpWavetable()
{
    wtSampHold.SetSampleRate(sampleRate);
    wtSampHold.PopulateWavetable();
}

void SampleAndHold::SetIncrement()
{
    wtSampHold.SetIncrement(modFrequency);
}
