/*
  ==============================================================================

    Wavetable.h
    Created: 26 Apr 2020 2:38:54pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once
#include "Oscillators.h"

class Wavetable
{
public:
    ~Wavetable()
    {
        delete[] sinWT;
    }
    
    /// Sets sampleRate and calls methodSetFrequency( )
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        setFrequency();
    }
    
    /// Populates wavetables with wave values using their populate functions
    void populateWavetables()
    {
        populateSinWT();
    }
    
    /// Main Playback of wavetable      BUST OUT INTO THREE SEPARATE for each wt?
    float process()
    {
        float wtOut = lagrangeInterpolation();
        readHeadPos += increment;
        
        if (readHeadPos > waveTableSize)
            readHeadPos -= waveTableSize;
        
        return wtOut;
    }
    
    /// Sets wavetable step increment based on note frequency
    void setIncrement(float noteFreq)
    {
        increment = ( noteFreq * waveTableSize ) / sampleRate;
    }
    
    
private:
    /// Sets frequency so one cycle is waveTableSize long at sampleRate
    void setFrequency()
    {
        frequency = sampleRate / waveTableSize;
    }
    
    /// Sets sampleRate and frequency for the sinOsc
    void sinOscSetup()
    {
        sinOsc.setSampleRate(sampleRate);
        sinOsc.setFrequency(frequency);
    }
    
    void populateSinWT()
    {
        sinOscSetup();
        
        for (int i=0; i<waveTableSize; i++)
        {
            sinWT[i] = sinOsc.process();
        }
    }
    
    /// Tom's interpolation method
    float lagrangeInterpolation()
    {
        float frac = readHeadPos - floor(readHeadPos);  // Position between indexes
        float sum = 0.0f;       // initialize sum
        
        for (int i=-2; i<2; i++)
        {
            int index = floor(readHeadPos + (i + 1));   // index after readHeadPos in time
            index %= waveTableSize;                     // wrap index
            float outVal = sinWT[index];// FIX THIS     // value at wt[index] to outVal
            float denominator = 1.0f;                   // initialize denominator
            
            for (int j=-2; j<2; j++)
            {
                if (j != i)
                {
                    outVal *= (frac - j);
                    denominator *= (i - j);
                }
            }
            
            sum += outVal / denominator;
        }
        
        return sum;
    }
    
    // Wavetable Parameters
    int waveTableSize = 1024;
    float frequency;
    float sampleRate;
    
    float readHeadPos = 0.0f;
    float increment;
    
    // Create buffer for wavetables
    float* sinWT = new float[waveTableSize];
    
    // Instance of oscillators
    SinOsc sinOsc;
};
