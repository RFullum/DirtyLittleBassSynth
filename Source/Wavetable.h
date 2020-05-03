/*
  ==============================================================================

    Wavetable.h
    Created: 26 Apr 2020 2:38:54pm
    Author:  B150987

 
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

/// Creates wavetable of a single Sine Wave cycle
class Wavetable
{
public:
    /// Constructor: initializes all samples in wavetable to 0.0f
    Wavetable()
    {
        for (int i=0; i<waveTableSize; i++)
        {
            waveTable[i] = 0.0;
        }
    }
    
    /// Destructo!
    virtual ~Wavetable()
    {
        delete[] waveTable;
    }
    
    /// Sets sampleRate and calls methodSetFrequency( )
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        setFrequency();
    }
    
    /// Populates wavetables with wave values using their private populate functions
    virtual void populateWavetable()
    {
        populateWT();
    }
    
    /// Main Playback of wavetable
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

    
protected:
    /// Returns maximum amplitude in wavetable argument
    float findMaxAmplitude(float* wt)
    {
        float currentMax = 0.0f;
        
        for (int i=0; i<waveTableSize; i++)
        {
            if ( fabsf(wt[i]) > currentMax )
            {
                currentMax = fabsf(wt[i]);
            }
        }
        
        return currentMax;
    }
    
    /// Normalizes wavetable between -1.0f and 1.0f
    void normalizeWaveTable()
    {
        float maxAmp = findMaxAmplitude(waveTable);
        
        for (int i=0; i<waveTableSize; i++)
        {
            waveTable[i] /= maxAmp;
        }
    }
    
    // Wavetable Parameters
    int waveTableSize = 1024;
    float sampleRate;
    float frequency;
    
    // Create buffer for wavetables
    float* waveTable = new float[waveTableSize];
    
    // Instance of oscillators
    SinOsc sinOsc;
    
    
private:
    /// Sets frequency so one cycle is waveTableSize long at sampleRate
    void setFrequency()
    {
        frequency = sampleRate / waveTableSize;
    }
    
    /// Sets sampleRate and frequency for the sinOsc
    virtual void oscSetup()
    {
        sinOsc.setSampleRate(sampleRate);
        sinOsc.setFrequency(frequency);
    }
    
    /// populates wavetable with values for a sine wave cycle
    virtual void populateWT()
    {
        oscSetup();
        
        for (int i=0; i<waveTableSize; i++)
        {
            waveTable[i] = sinOsc.process();
        }
    }
    
    /// Tom's interpolation method: returns interpolated wavetable sample values
    float lagrangeInterpolation()
    {
        float frac = readHeadPos - floor(readHeadPos);  // Position between indexes
        float sum = 0.0f;       // initialize sum
        
        for (int i=-2; i<2; i++)
        {
            int index = floor(readHeadPos + (i + 1));   // index after readHeadPos in time
            index %= waveTableSize;                     // wrap index
            float outVal = waveTable[index];            // value at wt[index] to outVal
            float denominator = 1.0f;                   // initialize denominator
            
            // interpolate algorithm
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
    
    // Playback parameters
    float readHeadPos = 0.0f;
    float increment;    // Increment controls speed of readHeadPos, controlling playback frequency
    
};


//========================================================================


// CHILD Class of Wavetable for band limited Sawtooth
/// Creates Sawtooth Wavetable using a number of amplitude adjusted sine waves at the harmonics
class SawWavetable : public Wavetable
{
public:
    /// Populates wavetables with wave values using their private populate functions
    void populateWavetable() override
    {
        populateSawWT();
    }
    
private:
    /// Adds instances of SinOsc into sawHarmonics OwnedArray
    void createHarmonics()
    {
        for (int i=0; i<numSawHarmonics; i++)
        {
            sawHarmonics.add( new SinOsc() );
        }
    }
    
    /// Sets sample rate for each SineOsc in saw harmonics
    void setSawSampleRates()
    {
        for (int i=0; i<numSawHarmonics; i++)
        {
            sawHarmonics[i]->setSampleRate(sampleRate);
        }
    }
    
    /// Sets frequency for each harmonic in the saw series
    void setSawFrequencies()
    {
        float harmonicFreq;
        
        for (int i=0; i<numSawHarmonics; i++)
        {
            if (i == 0)     // Fundamental 1/1 ratio
            {
                harmonicFreq = frequency;
            }
            else            // partials up the harmonic series ratios
            {
                harmonicFreq *= ( (i + 1.0f) / i );
            }

            sawHarmonics[i]->setFrequency(harmonicFreq);
        }
    }
    
    /// Runs process( ) function on each SinOsc and scales to the amplitude of that partial
    void sumHarmonics()
    {
        for (int i=0; i<waveTableSize; i++)
        {
            for (int j=0; j<numSawHarmonics; j++)
            {
                float harmonicAmplitude = 1.0f / (j + 1.0f);    // Amplitude of harmonic is 1/n where n is the harmonic number
                waveTable[i] += sawHarmonics[j]-> process() * harmonicAmplitude;
            }
        }
    }
    
    /// Populates wavetable with values for band limited saw wave
    void populateSawWT()
    {
        createHarmonics();
        setSawSampleRates();
        setSawFrequencies();
        sumHarmonics();
        normalizeWaveTable();
    }
    
    // Instance of oscillators
    OwnedArray<SinOsc> sawHarmonics;
    int numSawHarmonics = 57;   // Fundamental + 56 partials  -- Adjust this number to mod saw timbre
};


//========================================================================

//Child class of Wavetable for square wave
/// Creates Square Wavetable using a number of amplitude adjusted sine waves at the odd harmonics
class SquareWavetable : public Wavetable
{
public:
    /// Populates wavetables with wave values using their private populate functions
    void populateWavetable() override
    {
        populateSquareWT();
    }
    
protected:
    /// Adds instances of SinOsc into squareHarmonics OwnedArray
    void createHarmonics()
    {
        for (int i=0; i<numSquareHarmonics; i++)
        {
            squareHarmonics.add( new SinOsc() );
        }
    }
    
    /// Sets sample rate for each SineOsc in saw harmonics
    void setSquareSampleRates()
    {
        for (int i=0; i<numSquareHarmonics; i++)
        {
            squareHarmonics[i]->setSampleRate(sampleRate);
        }
    }
    
    /** Sets frequency for each harmonicin the Square series by finding all the harmonics from
     the fundamental to double the number of harmonics, then using the odd harmonics to set the
     frequency for each sine instance
     */
    void setSquareFrequencies()
    {
        int twiceOddHarmonics = numSquareHarmonics * 2;
        float* harmonicFreq = new float[twiceOddHarmonics];
        
        for (int i=0; i<twiceOddHarmonics; i++)
        {
            if (i == 0)     // Fundamental 1/1
            {
                harmonicFreq[i] = frequency;
            }
            else            // Harmonic series ratios
            {
                harmonicFreq[i] = harmonicFreq[i-1] * ( (i + 1.0f) / i );
            }
            
            if (i % 2 == 0) // Use the odd harmonics (at the even indexes) to set the frequencies
            {
                squareHarmonics[i / 2]->setFrequency(harmonicFreq[i]);
            }
        }
    }
    
    /// Runs process( ) function on each SinOsc and scales to the amplitude of that partial
    void sumHarmonics()
    {
        for (int i=0; i<waveTableSize; i++)
        {
            for (int j=0; j<numSquareHarmonics; j++)
            {
                float harmonicAmplitude = 1.0f / (j * 2.0f + 1.0f);    // Amplitude of harmonic is 1/n where n is the harmonic number
                waveTable[i] += squareHarmonics[j]-> process() * harmonicAmplitude;
            }
        }
    }
    
private:
    /// Populates wavetable with values for band limited square wave
    virtual void populateSquareWT()
    {
        createHarmonics();
        setSquareSampleRates();
        setSquareFrequencies();
        sumHarmonics();
        normalizeWaveTable();
    }
    
    // Instance of oscillators
    OwnedArray<SinOsc> squareHarmonics;
    int numSquareHarmonics = 57;    // Fundamental + 56 partials -- Adjust this number to mod square timbre
    
};



//========================================================================

// Child class of SquareWavetable for SpikeWave
/// Creates a Spike wavetable by highpassing a square wavetable
class SpikeWavetable : public SquareWavetable
{
private:
    // Highpasses wavetable
    void highPassSpike()
    {
        cutoffFreq = frequency * 15.0f;
        
        highPass.reset();
        highPass.setCoefficients( IIRCoefficients::makeHighPass(sampleRate, cutoffFreq) );
        
        for (int i=0; i<waveTableSize; i++)
        {
            waveTable[i] = highPass.processSingleSampleRaw(waveTable[i]);
        }
        
    }
    
    /// Populates wavetable with Spike derived from band limited square wavetable
    void populateSquareWT() override
    {
        createHarmonics();
        setSquareSampleRates();
        setSquareFrequencies();
        sumHarmonics();
        highPassSpike();
        normalizeWaveTable();
    }
    
    // Highpass members
    IIRFilter highPass;
    float cutoffFreq;
};
