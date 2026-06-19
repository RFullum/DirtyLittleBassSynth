/*
  ==============================================================================

    Modifiers.h
    Created: 29 Apr 2020 10:30:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/Wavetable.h"
#include "DryWet.h"

//==============================================================================

class RingMod
{
public:
    RingMod();

    void  SetSampleRate(float SR);
    void  ModFreq(float fqncy, float offset);
    void  SetRingToneSlider(float toneSlider);
    float Process();

private:
    void SetUpWavetables();
    void SetIncrement();

    float sampleRate   = 44100.0f;
    float modFrequency = 0.0f;

    FullumOscillators::Wavetable       wtSine;
    FullumOscillators::SquareWavetable wtSquare;

    float ringToneSlider = 0.0f;
};

//============================================================

class FrequencyShifter
{
public:
    FrequencyShifter();

    void  SetSampleRate(float SR);
    void  ModFreq(float fqncy, float offset);
    void  OscMorph(float sinLevelIn, float spikeLevelIn, float sawLevelIn);
    float Process();

private:
    void SetUpWavetables();
    void SetIncrement();

    float sampleRate   = 44100.0f;
    float modFrequency = 0.0f;

    FullumOscillators::Wavetable      wtSine;
    FullumOscillators::SpikeWavetable wtSpike;
    FullumOscillators::SawWavetable   wtSaw;

    float sineLevel  = 1.0f;
    float spikeLevel = 0.0f;
    float sawLevel   = 0.0f;
};

//============================================================

class SampleAndHold
{
public:
    SampleAndHold();

    void  SetSampleRate(float SR);
    void  ModFreq(float fqncy, float offset);
    float ProcessSH(float oscSampleValIn);

private:
    void SetUpWavetable();
    void SetIncrement();

    float sampleRate   = 44100.0f;
    float modFrequency = 0.0f;

    FullumOscillators::SquareWavetable wtSampHold;

    float oscSampleVal  = 0.0f;
    float holdSampleVal = 0.0f;
};
