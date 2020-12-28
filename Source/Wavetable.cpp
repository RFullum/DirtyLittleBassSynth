/*
  ==============================================================================

    Wavetable.cpp
    Created: 26 Dec 2020 3:25:26pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Wavetable.h"


//
//=== Wavetable Class: Sine Wave =================================
//

/// Constructor: initializes all samples in wavetable to 0.0f
Wavetable::Wavetable() : waveTableSize(1024), readHeadPos(0.0f)
{
    for (int i=0; i<waveTableSize; i++)
    {
        waveTable[i] = 0.0;
    }
}

/// Destructo!
Wavetable::~Wavetable()
{
    delete[] waveTable;
}

/// Sets sampleRate and calls methodSetFrequency( )
void Wavetable::setSampleRate(float SR)
{
    sampleRate = SR;
    setFrequency();
}

/// Populates wavetables with wave values using their private populate functions
void Wavetable::populateWavetable()
{
    populateWT();
}

/// Main Playback of wavetable
float Wavetable::process()
{
    float wtOut = lagrangeInterpolation();
    readHeadPos += increment;
    
    if (readHeadPos > waveTableSize)
        readHeadPos -= waveTableSize;
    
    return wtOut;
}

/// Sets wavetable step increment based on note frequency
void Wavetable::setIncrement(float noteFreq)
{
    increment = ( noteFreq * waveTableSize ) / sampleRate;
}

/// Returns maximum amplitude in wavetable argument
float Wavetable::findMaxAmplitude(float* wt)
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
void Wavetable::normalizeWaveTable()
{
    float maxAmp = findMaxAmplitude(waveTable);
    
    for (int i=0; i<waveTableSize; i++)
    {
        waveTable[i] /= maxAmp;
    }
}

/// Sets frequency so one cycle is waveTableSize long at sampleRate
void Wavetable::setFrequency()
{
    frequency = sampleRate / waveTableSize;
}

/// Sets sampleRate and frequency for the sinOsc
void Wavetable::oscSetup()
{
    sinOsc.setSampleRate(sampleRate);
    sinOsc.setFrequency(frequency);
}

/// populates wavetable with values for a sine wave cycle
void Wavetable::populateWT()
{
    oscSetup();
    
    for (int i=0; i<waveTableSize; i++)
    {
        waveTable[i] = sinOsc.process();
    }
}

/// Returns interpolated wavetable sample values
float Wavetable::lagrangeInterpolation()
{
    float frac = readHeadPos - floor(readHeadPos);  // Position between indexes
    float sum  = 0.0f;                              // initialize sum
    
    for (int i=-2; i<2; i++)
    {
        int index = floor(readHeadPos + (i + 1));   // index after readHeadPos in time
        
        index %= waveTableSize;                     // wrap index
        
        float outVal      = waveTable[index];            // value at wt[index] to outVal
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


//
//=== SawWavetable Class ================================================
//

// CHILD Class of Wavetable for band limited Sawtooth

/// Creates Sawtooth Wavetable using a number of amplitude adjusted sine waves at the harmonics
SawWavetable::SawWavetable() : numSawHarmonics(57) {} // Fundamental + 56 partials  -- Adjust this number to mod saw timbre

/// Populates wavetables with wave values using their private populate functions
void SawWavetable::populateWavetable()
{
    populateSawWT();
}

/// Adds instances of SinOsc into sawHarmonics OwnedArray
void SawWavetable::createHarmonics()
{
    for (int i=0; i<numSawHarmonics; i++)
    {
        sawHarmonics.add( new SinOsc() );
    }
}

/// Sets sample rate for each SineOsc in saw harmonics
void SawWavetable::setSawSampleRates()
{
    for (int i=0; i<numSawHarmonics; i++)
    {
        sawHarmonics[i]->setSampleRate(sampleRate);
    }
}

/// Sets frequency for each harmonic in the saw series
void SawWavetable::setSawFrequencies()
{
    float harmonicFreq;
    
    for (int i=0; i<numSawHarmonics; i++)
    {
        if (i == 0)                                 // Fundamental 1/1 ratio
        {
            harmonicFreq = frequency;
        }
        else                                        // partials up the harmonic series ratios
        {
            harmonicFreq *= ( (i + 1.0f) / i );
        }

        sawHarmonics[i]->setFrequency(harmonicFreq);
    }
}

/// Runs process( ) function on each SinOsc and scales to the amplitude of that partial
void SawWavetable::sumHarmonics()
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
void SawWavetable::populateSawWT()
{
    createHarmonics();
    setSawSampleRates();
    setSawFrequencies();
    sumHarmonics();
    normalizeWaveTable();
}


//
//=== SquareWavetable Class ================================================
//

/// Creates Square Wavetable using a number of amplitude adjusted sine waves at the odd harmonics
SquareWavetable::SquareWavetable() : numSquareHarmonics(57) {}  // Fundamental + 56 partials -- Adjust this number to mod square timbre

/// Populates wavetables with wave values using their private populate functions
void SquareWavetable::populateWavetable()
{
    populateSquareWT();
}

/// Adds instances of SinOsc into squareHarmonics OwnedArray
void SquareWavetable::createHarmonics()
{
    for (int i=0; i<numSquareHarmonics; i++)
    {
        squareHarmonics.add( new SinOsc() );
    }
}

/// Sets sample rate for each SineOsc in saw harmonics
void SquareWavetable::setSquareSampleRates()
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
void SquareWavetable::setSquareFrequencies()
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
void SquareWavetable::sumHarmonics()
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

/// Populates wavetable with values for band limited square wave
void SquareWavetable::populateSquareWT()
{
    createHarmonics();
    setSquareSampleRates();
    setSquareFrequencies();
    sumHarmonics();
    normalizeWaveTable();
}


//
//=== SpikeWavetable Class ================================================
//

/// Highpasses wavetable
void SpikeWavetable::highPassSpike()
{
    cutoffFreq = frequency * 10.0f;
    
    highPass.reset();
    highPass.setCoefficients( IIRCoefficients::makeHighPass(sampleRate, cutoffFreq, 2.0f) );
    
    for (int i=0; i<waveTableSize; i++)
    {
        waveTable[i] = highPass.processSingleSampleRaw(waveTable[i]);
    }
    
}

/// Populates wavetable with Spike derived from band limited square wavetable
void SpikeWavetable::populateSquareWT()
{
    createHarmonics();
    setSquareSampleRates();
    setSquareFrequencies();
    sumHarmonics();
    highPassSpike();
    normalizeWaveTable();
}

