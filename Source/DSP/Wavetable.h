/*
  ==============================================================================

    Wavetable.h
    Created: 26 Apr 2020 2:38:54pm
    Author:  Robert Fullum

 
 Parent Class: Wavetable
 -Creates a sine wave wavetable
 
 Children:
 -SawWavetable
 -SquareWavetable
 
 Grandchildren:
 -SpikeWavetable (child of SquareWavetable)
  ==============================================================================
*/

#pragma once
#include "Oscillators.h"

//=== Wavetable Class: Sine Wave =================================

class Wavetable
{
public:
    Wavetable();
    virtual ~Wavetable();
    
    void SetSampleRate(float SR);
    
    virtual void PopulateWavetable();
    
    float Process();
    void  SetIncrement(float noteFreq);
    float GetWavetableSampleValue(int index);
    
protected:
    float FindMaxAmplitude(float *wt);
    void  NormalizeWaveTable(); // -1 to 1
    
    int   waveTableSize;
    float sampleRate;
    float frequency;
    
    float *waveTable = new float[waveTableSize];
    
    SinOsc sinOsc;
    
private:
    void SetFrequency();
    
    virtual void OscSetup();
    virtual void PopulateWT();
    
    float LagrangeInterpolation();
    
    float readHeadPos;
    float increment;    // Increment controls speed of readHeadPos, controlling playback frequency
    
};

//========================================================================

class SawWavetable
    : public Wavetable
{
public:
    SawWavetable();
    ~SawWavetable();
    
    void PopulateWavetable() override;
    
private:
    void CreateHarmonics();
    void SetSawSampleRates();
    void SetSawFrequencies();
    void SumHarmonics();
    void PopulateSawWT();
    
    juce::OwnedArray<SinOsc> sawHarmonics;
    int numSawHarmonics; // Fundamental + 56 partials  -- Adjust this number to mod saw timbre
};

//========================================================================

class SquareWavetable
    : public Wavetable
{
public:
    /// Constructor & Destructor
    SquareWavetable();
    ~SquareWavetable();
    
    void PopulateWavetable() override;
    
protected:
    void CreateHarmonics();
    void SetSquareSampleRates();
    void SetSquareFrequencies();
    void SumHarmonics();
    
private:
    virtual void PopulateSquareWT();
    
    juce::OwnedArray<SinOsc> squareHarmonics;
    int numSquareHarmonics; // Fundamental + 56 partials -- Adjust this number to mod square timbre
    
};

//========================================================================

class SpikeWavetable
    : public SquareWavetable
{
private:
    void HighPassSpike();
    void PopulateSquareWT() override;
    
    juce::IIRFilter highPass;
    float cutoffFreq;
};
