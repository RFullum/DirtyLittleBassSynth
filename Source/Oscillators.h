//
//  Oscillators.h
//  ClassClass
//
//  Created by B150987
//  Copyright © 2020 
//

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>


// PARENT phasor class
class Phasor
{
public:
    
    // Our parent oscillator class does the key things required for most oscillators:
    // -- handles phase
    // -- handles setters and getters for frequency and samplerate
    
    /// virtual desctructor
    virtual ~Phasor() {};
    
    /// update the phase and output the next sample from the oscillator
    float process()
    {
        phase += phaseDelta;
        
        if (phase > 1.0f)
            phase -= 1.0f;
        
        return output(phase);
    }
    
    /// Phase
    virtual float output(float p)
    {
        return p;
    }
    
    /// Sets sampleRate
    void setSampleRate(float SR)
    {
        sampleRate = SR;
    }
    
    /// Sets oscillator frequency
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }
    
    // TESTING
    
    float getFreq()
    {
        return frequency;
    }
    
    float getSR()
    {
        return sampleRate;
    }
    
    
private:
    float frequency;
    float sampleRate;
    float phase = 0.0f;
    float phaseDelta;
};
//==========================================


// CHILD Class
class TriOsc : public Phasor
{
    float output(float p) override
    {
        return fabsf(p - 0.5f) - 0.5f;
    }
};


// CHILD Class
class SinOsc : public Phasor
{
    float output(float p) override
    {
        return std::sin(p * TWOPI);
    }
    
private:
    float TWOPI = 2.0f * M_PI;
};

// CHILD Class
class SquareOsc : public Phasor
{
public:
    float output(float p) override
    {
        float outVal = 0.5;
        if (p > pulseWidth)
            outVal = -0.5;
        return outVal;
    }
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }
private:
    float pulseWidth = 0.5f;
};

// Child Class
class SawtoothOsc : public Phasor
{
    float output(float p) override
    {
        return p -= 0.5f;
    }
};


// Child Class
/// Sine wave, silent after the pulseWidth
class SparseLFO : public Phasor
{
public:
    float output(float p) override
    {
        float outVal = sin(p * 0.5f * TWOPI / pulseWidth);
        if (p > pulseWidth)
            outVal = 0.0f;
        return outVal*outVal;
    }
    
    /// Sets pulseWidth: float between 0.0 and 1.0
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }
        
private:
    float pulseWidth = 0.5f;
    float TWOPI = 2.0f * M_PI;
};

// CHILD Class
/// Outputs 1.0f for pulseWidth, then outputs 0.0f
class SquareIOLFO : public Phasor
{
public:
    float output(float p) override
    {
        float outVal = 1.0f;
        if (p > pulseWidth)
            outVal = 0.0f;
        return outVal;
    }
    
    /// sets pulseWidth between 0.0f and 1.0f
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }
private:
    float pulseWidth = 0.5f;
};


