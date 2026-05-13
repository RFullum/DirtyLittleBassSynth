/*
  ==============================================================================

    Oscillators.cpp
    Created: 26 Dec 2020 2:42:00pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Oscillators.h"

//=== Phasor Class ===================================================

Phasor::Phasor() = default;

float Phasor::process()
{
    phase += phaseDelta;

    if (phase > 1.0f)
        phase -= 1.0f;

    return output(phase);
}

float Phasor::output(float p)
{
    return p;
}

void Phasor::setSampleRate(float SR)
{
    sampleRate = SR;
}

void Phasor::setFrequency(float freq)
{
    frequency  = freq;
    phaseDelta = frequency / sampleRate;
}

float Phasor::getFreq() { return frequency;  }
float Phasor::getSR()   { return sampleRate; }

//=== TriOsc Class ===================================================

float TriOsc::output(float p)
{
    return fabsf(p - 0.5f) - 0.5f;
}

//=== SinOsc Class ===================================================

SinOsc::SinOsc() = default;

float SinOsc::output(float p)
{
    return std::sin(p * TWOPI);
}

//=== SquareOsc Class ===================================================

SquareOsc::SquareOsc() = default;

float SquareOsc::output(float p)
{
    float outVal = 0.5;

    if (p > pulseWidth)
        outVal = -0.5;

    return outVal;
}

void SquareOsc::setPulseWidth(float pw)
{
    pulseWidth = pw;
}

//=== SawtoothOsc Class ===================================================

float SawtoothOsc::output(float p)
{
    return p -= 0.5f;
}

//=== SparseLFO Class ===================================================

SparseLFO::SparseLFO() = default;

float SparseLFO::output(float p)
{
    float outVal = sin(p * 0.5f * TWOPI / pulseWidth);

    if (p > pulseWidth)
        outVal = 0.0f;

    return outVal * outVal;
}

void SparseLFO::setPulseWidth(float pw)
{
    pulseWidth = pw;
}

//=== SquareIOLFO Class ===================================================

SquareIOLFO::SquareIOLFO() = default;

float SquareIOLFO::output(float p)
{
    float outVal = 1.0f;

    if (p > pulseWidth)
        outVal = 0.0f;

    return outVal;
}

void SquareIOLFO::setPulseWidth(float pw)
{
    pulseWidth = pw;
}
