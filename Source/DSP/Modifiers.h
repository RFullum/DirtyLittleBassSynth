/*
  ==============================================================================

    Modifiers.h
    Created: 29 Apr 2020 10:30:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Wavetable.h"
#include "DryWet.h"
#include "OscillatorParameterControls.h"

//==============================================================================

class RingMod
{
public:
    RingMod();
    virtual ~RingMod();

    virtual void SetSampleRate(float SR);
    virtual void ModFreq(float fqncy, float offset);

    void SetRingToneSlider(float toneSlider);

    virtual float Process();

protected:
    float sampleRate;
    float modFrequency;

private:
    void  SetUpWavetables();
    void  SetRingIncrement();
    float RingModProcess();

    Wavetable wtSine;
    SquareWavetable wtSquare;

    float ringToneSlider;
};

//============================================================

class FrequencyShifter
    : public RingMod
{
public:
    FrequencyShifter();

    void  SetSampleRate(float SR) override;
    void  ModFreq(float fqncy, float offset) override;
    void  OscMorph(float sinLevelIn, float spikeLevelIn, float sawLevelIn);
    float Process() override;

private:
    void  SetUpFreqShiftWavetables();
    void  SetFreqShiftIncrement();
    float FreqShiftProcess();

    Wavetable wtSine;
    SpikeWavetable wtSpike;
    SawWavetable wtSaw;

    float sineLevel;
    float spikeLevel;
    float sawLevel;
};

//============================================================

class SampleAndHold
    : public RingMod
{
public:
    SampleAndHold();

    void  SetSampleRate(float SR) override;
    void  ModFreq(float fqncy, float offset) override;
    float ProcessSH(float oscSampleValIn);

private:
    void  SetSampHoldWavetables();
    void  SetSampHoldIncrement();
    float SampleHoldProcess();

    SquareWavetable wtSampHold;

    float oscSampleVal;
    float holdSampleVal;
};
