/*
  ==============================================================================

    Wavetable.h
    Created: 26 Apr 2020 2:38:54pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once
#include "Oscillators.h"

// Creates wavetable of a single Sine Wave cycle
class Wavetable
{
public:
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
    
    /// Populates wavetables with wave values using their populate functions
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
    /// finds maximum amplitude in wavetable argument
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
    
    /*      UNUSED???
    /// Finds minimum amplitude in wavetable argument
    float findMinAmplitude(float* wt)
    {
        float currentMin = 0.0f;
        
        for (int i=0; i<waveTableSize; i++)
        {
            if (wt[i] < currentMin)
            {
                currentMin = wt[i];
            }
        }
        
        return currentMin;
    }
    */
    
    
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
    
    /// Tom's interpolation method
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
    float increment;

    
};


//========================================================================


// CHILD Class for band limited Sawtooth
class SawWavetable : public Wavetable
{
public:
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
            if (i == 0)
            {
                harmonicFreq = frequency;
            }
            else
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
    
    /// Normalizes wavetable between -1.0f and 1.0f
    void normalizeWaveTable()
    {
        float maxAmp = findMaxAmplitude(waveTable);
        
        for (int i=0; i<waveTableSize; i++)
        {
            waveTable[i] /= maxAmp;
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

//Child class for spike wave (high-passed square wave)
class SpikeWavetable : public Wavetable
{
public:
    void populateWavetable() override
    {
        populateSpikeWT();
    }
    
private:
    // Adds instances of SinOsc into spikeHarmonics OwnedArray
    void createHarmonics()
    {
        for (int i=0; i<numSpikeHarmonics; i++)
        {
            spikeHarmonics.add( new SinOsc() );
        }
    }
    
    /// Sets sample rate for each SineOsc in saw harmonics
    void setSpikeSampleRates()
    {
        for (int i=0; i<numSpikeHarmonics; i++)
        {
            spikeHarmonics[i]->setSampleRate(sampleRate);
        }
    }
    
    /// Sets frequency for each harmonic in the saw series
    void setSpikeFrequencies()
    {
        int twiceOddHarmonics = numSpikeHarmonics * 2;
        float* harmonicFreq = new float[twiceOddHarmonics];
        
        for (int i=0; i<twiceOddHarmonics; i++)
        {
            if (i == 0)
            {
                harmonicFreq[i] = frequency;
            }
            else
            {
                harmonicFreq[i] = harmonicFreq[i-1] * ( (i + 1.0f) / i );
            }
            
            if (i % 2 == 0)
            {
                spikeHarmonics[i / 2]->setFrequency(harmonicFreq[i]);
            }
        }
    }
    
    /// Runs process( ) function on each SinOsc and scales to the amplitude of that partial
    void sumHarmonics()
    {
        for (int i=0; i<waveTableSize; i++)
        {
            for (int j=0; j<numSpikeHarmonics; j++)
            {
                float harmonicAmplitude = 1.0f / (j * 2.0f + 1.0f);    // Amplitude of harmonic is 1/n where n is the harmonic number
                waveTable[i] += spikeHarmonics[j]-> process() * harmonicAmplitude;
            }
        }
    }
    
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
    
    void populateSpikeWT()
    {
        createHarmonics();
        setSpikeSampleRates();
        setSpikeFrequencies();
        sumHarmonics();
        highPassSpike();
        
        /*
         To Do:
         -Normalize
         -Change both normalizations to Matthew's methods
         */
    }
    
    // Instance of oscillators
    OwnedArray<SinOsc> spikeHarmonics;
    int numSpikeHarmonics = 13;    // Fundamental + 12 partials -- Adjust this number to mod square timbre
    
    // Highpass members
    IIRFilter highPass;
    float cutoffFreq;
};
