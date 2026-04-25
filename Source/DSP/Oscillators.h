//
//  Oscillators.h
//  ClassClass
//
//  Created by Robert Fullum
//  Copyright © 2020 
//

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <JuceHeader.h>


//
//=== Phasor Class ===================================================
//

class Phasor
{
public:
    
    // Our parent oscillator class does the key things required for most oscillators:
    // -- handles phase
    // -- handles setters and getters for frequency and samplerate
    
    /// Constructor
    Phasor();
    
    /// virtual desctructor
    virtual ~Phasor();
    
    /// update the phase and output the next sample from the oscillator
    float process();
    
    /// Phase
    virtual float output(float p);
    
    /// Sets sampleRate
    void setSampleRate(float SR);
    
    /// Sets oscillator frequency
    void setFrequency(float freq);
    
    // TESTING
    /// Returns frequency
    float getFreq();
    
    /// Returns sampleRate
    float getSR();
    
    
private:
    float frequency;
    float sampleRate;
    float phase;
    float phaseDelta;
};


//
//=== TriOsc Class ===================================================
//

class TriOsc : public Phasor
{
    float output(float p) override;
};


//
//=== SinOsc Class ===================================================
//

class SinOsc : public Phasor
{
public:
    /// Constructor
    SinOsc();
    
    float output(float p) override;
    
private:
    float TWOPI;
};


//
//=== SquareOsc Class ===================================================
//

class SquareOsc : public Phasor
{
public:
    /// Constructor
    SquareOsc();
    
    float output(float p) override;
    
    /// Sets pulswidth
    void setPulseWidth(float pw);
    
private:
    float pulseWidth;
};


//
//=== SawtoothOsc Class ===================================================
//
class SawtoothOsc : public Phasor
{
    float output(float p) override;
    
};


//
//=== SparseLFO Class ===================================================
//

/// Sine wave, silent after the pulseWidth
class SparseLFO : public Phasor
{
public:
    /// Constructor
    SparseLFO();
    
    float output(float p) override;
    
    /// Sets pulseWidth: float between 0.0 and 1.0
    void setPulseWidth(float pw);

        
private:
    float pulseWidth;
    float TWOPI;
};


//
//=== SquareIOLFO Class ===================================================
//

/// Outputs 1.0f for pulseWidth, then outputs 0.0f
class SquareIOLFO : public Phasor
{
public:
    /// Constructor
    SquareIOLFO();
    
    float output(float p) override;
    
    /// sets pulseWidth between 0.0f and 1.0f
    void setPulseWidth(float pw);
    
    
private:
    float pulseWidth;
};


