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
            if (wt[i] > currentMax)
            {
                currentMax = wt[i];
            }
        }
        
        return currentMax;
    }
    
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
            //std::cout << "\nsawHarmonic" << i << " " << sawHarmonics[i]->getSR();
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
            std::cout << "\nharmonicFreq" << i << " " << harmonicFreq;
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
    
    //
    //      CONTINUE BREAKING OUT LOOPS INTO SEPERATE FUNCTIONS
    //
    /// Populates wavetable with values for band limited saw wave
    void populateSawWT()
    {
        createHarmonics();
        setSawSampleRates();
        setSawFrequencies();
        sumHarmonics();
        
        /*
        FIX THE EXC_BAD_ADDRESS ISSUE THEN UNCOMMENT
         
        // Get min and max amplitudes in wavetable
        float minAmp = findMinAmplitude(waveTable);
        float maxAmp = findMaxAmplitude(waveTable);
        std::cout << "\nThis is the Min Amplitude " << minAmp << "\n";
        std::cout << "\nThis is the Max Amplitude " << maxAmp << "\n";
        // Normalize between -1.0f and 1.0f
        for (int i=0; i<waveTableSize; i++)
        {
            waveTable[i] = jmap(waveTable[i], minAmp, maxAmp, -1.0f, 1.0f);
        }
        */
    }
    
    // Instance of oscillators
    OwnedArray<SinOsc> sawHarmonics;    
    int numSawHarmonics = 57;   // Fundamental + 56 partials  -- Adjust this number to mod saw timbre
};
