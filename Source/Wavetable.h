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


//
//=== Wavetable Class: Sine Wave =================================
//


/// Creates wavetable of a single Sine Wave cycle
class Wavetable
{
public:
    /// Constructor: initializes all samples in wavetable to 0.0f
    Wavetable();
    
    /// Destructo!
    virtual ~Wavetable();
    
    /// Sets sampleRate and calls methodSetFrequency( )
    void setSampleRate(float SR);
    
    /// Populates wavetables with wave values using their private populate functions
    virtual void populateWavetable();
    
    /// Main Playback of wavetable
    float process();
    
    /// Sets wavetable step increment based on note frequency
    void setIncrement(float noteFreq);
    
    /// Returns the sample value at the index given in the argument
    float getWavetableSampleValue(int index);

    
protected:
    /// Returns maximum amplitude in wavetable argument
    float findMaxAmplitude(float* wt);
    
    /// Normalizes wavetable between -1.0f and 1.0f
    void normalizeWaveTable();
    
    // Wavetable Parameters
    int waveTableSize;
    float sampleRate;
    float frequency;
    
    // Create buffer for wavetables
    float* waveTable = new float[waveTableSize];
    
    // Instance of oscillators
    SinOsc sinOsc;
    
    
private:
    /// Sets frequency so one cycle is waveTableSize long at sampleRate
    void setFrequency();
    
    /// Sets sampleRate and frequency for the sinOsc
    virtual void oscSetup();
    
    /// populates wavetable with values for a sine wave cycle
    virtual void populateWT();
    
    /// Returns interpolated wavetable sample values
    float lagrangeInterpolation();
    
    // Playback parameters
    float readHeadPos;
    float increment;    // Increment controls speed of readHeadPos, controlling playback frequency
    
};


//========================================================================


// CHILD Class of Wavetable for band limited Sawtooth
class SawWavetable : public Wavetable
{
public:
    /// Creates Sawtooth Wavetable using a number of amplitude adjusted sine waves at the harmonics
    SawWavetable();
    ~SawWavetable();
    
    /// Populates wavetables with wave values using their private populate functions
    void populateWavetable() override;
    
private:
    /// Adds instances of SinOsc into sawHarmonics OwnedArray
    void createHarmonics();
    
    /// Sets sample rate for each SineOsc in saw harmonics
    void setSawSampleRates();
    
    /// Sets frequency for each harmonic in the saw series
    void setSawFrequencies();
    
    /// Runs process( ) function on each SinOsc and scales to the amplitude of that partial
    void sumHarmonics();
    
    /// Populates wavetable with values for band limited saw wave
    void populateSawWT();
    
    // Instance of oscillators
    OwnedArray<SinOsc> sawHarmonics;
    int numSawHarmonics; // Fundamental + 56 partials  -- Adjust this number to mod saw timbre
};


//========================================================================

//Child class of Wavetable for square wave
/// Creates Square Wavetable using a number of amplitude adjusted sine waves at the odd harmonics
class SquareWavetable : public Wavetable
{
public:
    /// Constructor & Destructor
    SquareWavetable();
    ~SquareWavetable();
    
    /// Populates wavetables with wave values using their private populate functions
    void populateWavetable() override;
    
protected:
    /// Adds instances of SinOsc into squareHarmonics OwnedArray
    void createHarmonics();
    
    /// Sets sample rate for each SineOsc in square harmonics
    void setSquareSampleRates();
    
    /** Sets frequency for each harmonicin the Square series by finding all the harmonics from
     the fundamental to double the number of harmonics, then using the odd harmonics to set the
     frequency for each sine instance
     */
    void setSquareFrequencies();
    
    /// Runs process( ) function on each SinOsc and scales to the amplitude of that partial
    void sumHarmonics();
    
private:
    /// Populates wavetable with values for band limited square wave
    virtual void populateSquareWT();
    
    // Instance of oscillators
    OwnedArray<SinOsc> squareHarmonics;
    int numSquareHarmonics; // Fundamental + 56 partials -- Adjust this number to mod square timbre
    
};



//========================================================================

// Child class of SquareWavetable for SpikeWave
/// Creates a Spike wavetable by highpassing a square wavetable
class SpikeWavetable : public SquareWavetable
{
private:
    /// Highpasses wavetable
    void highPassSpike();
    
    /// Populates wavetable with Spike derived from band limited square wavetable
    void populateSquareWT() override;
    
    // Highpass members
    IIRFilter highPass;
    float cutoffFreq;
};
