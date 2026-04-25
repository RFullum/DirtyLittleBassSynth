//
//  Oscillators.h
//  Created by Robert Fullum
//

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <JuceHeader.h>


//=== Phasor Class ===================================================

class Phasor
{
public:
    Phasor();
    virtual ~Phasor();

    float         process();
    virtual float output(float p);

    void setSampleRate(float SR);
    void setFrequency(float freq);

    float getFreq();
    float getSR();

private:
    float frequency;
    float sampleRate;
    float phase;
    float phaseDelta;
};


//=== TriOsc Class ===================================================

class TriOsc : public Phasor
{
    float output(float p) override;
};


//=== SinOsc Class ===================================================

class SinOsc : public Phasor
{
public:
    SinOsc();

    float output(float p) override;

private:
    float TWOPI;
};


//=== SquareOsc Class ===================================================

class SquareOsc : public Phasor
{
public:
    SquareOsc();

    float output(float p) override;

    void setPulseWidth(float pw);

private:
    float pulseWidth;
};


//=== SawtoothOsc Class ===================================================

class SawtoothOsc : public Phasor
{
    float output(float p) override;
};


//=== SparseLFO Class ===================================================

/// Sine wave, silent after the pulseWidth.
class SparseLFO : public Phasor
{
public:
    SparseLFO();

    float output(float p) override;

    void setPulseWidth(float pw);   // 0.0 .. 1.0

private:
    float pulseWidth;
    float TWOPI;
};


//=== SquareIOLFO Class ===================================================

/// Outputs 1.0f for pulseWidth, then 0.0f.
class SquareIOLFO : public Phasor
{
public:
    SquareIOLFO();

    float output(float p) override;

    void setPulseWidth(float pw);   // 0.0 .. 1.0

private:
    float pulseWidth;
};
