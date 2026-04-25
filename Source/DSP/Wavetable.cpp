/*
  ==============================================================================

    Wavetable.cpp
    Created: 26 Dec 2020 3:25:26pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Wavetable.h"

//=== Wavetable Class: Sine Wave =================================

Wavetable::Wavetable()
: waveTableSize(1024)
, sampleRate(44100.0f)
, frequency(0.0f)
, readHeadPos(0.0f)
, increment(1.0f)
{
    for (int i=0; i<waveTableSize; ++i)
        waveTable[i] = 0.0;
}

Wavetable::~Wavetable()
{
    delete[] waveTable;
}

void Wavetable::SetSampleRate(float SR)
{
    sampleRate = SR;
    SetFrequency();
}

void Wavetable::PopulateWavetable()
{
    PopulateWT();
}

float Wavetable::Process()
{
    float wtOut = LagrangeInterpolation();
    
    readHeadPos += increment;
    
    if (readHeadPos >= waveTableSize)
        readHeadPos -= waveTableSize;
    
    return wtOut;
}

void Wavetable::SetIncrement(float noteFreq)
{
    increment = ( noteFreq * waveTableSize ) / sampleRate;
}

float Wavetable::GetWavetableSampleValue(int index)
{
    return waveTable[index];
}

float Wavetable::FindMaxAmplitude(float* wt)
{
    float currentMax = 0.0f;
    
    for (int i=0; i<waveTableSize; i++)
        if ( fabsf(wt[i]) > currentMax )
            currentMax = fabsf(wt[i]);
    
    return currentMax;
}

void Wavetable::NormalizeWaveTable()
{
    float maxAmp = FindMaxAmplitude(waveTable);
    
    for (int i=0; i<waveTableSize; i++)
        waveTable[i] /= maxAmp;
}

void Wavetable::SetFrequency()
{
    frequency = sampleRate / waveTableSize;
}

void Wavetable::OscSetup()
{
    sinOsc.setSampleRate(sampleRate);
    sinOsc.setFrequency(frequency);
}

void Wavetable::PopulateWT()
{
    OscSetup();
    
    for (int i=0; i<waveTableSize; i++)
        waveTable[i] = sinOsc.process();
}

float Wavetable::LagrangeInterpolation()
{
    float frac = readHeadPos - floor(readHeadPos);  // fractional position between indexes
    float sum  = 0.0f;

    for (int i = -2; i < 2; i++)
    {
        int index = floor(readHeadPos + (i + 1));
        index %= waveTableSize;

        if (index < 0.0f)
            index += waveTableSize;

        float outVal      = waveTable[index];
        float denominator = 1.0f;

        for (int j = -2; j < 2; j++)
        {
            if (j != i)     // denominator can't be 0
            {
                outVal      *= (frac - j);
                denominator *= (i - j);
            }
        }

        sum += outVal / denominator;
    }

    return sum;
}


//=== SawWavetable Class ================================================

SawWavetable::SawWavetable()
: numSawHarmonics(57) // Fundamental + 56 partials  -- Adjust this number to mod saw timbre

{}

SawWavetable::~SawWavetable()
{
    for (int i=0; i<numSawHarmonics; ++i)
        sawHarmonics.remove(i);
}

void SawWavetable::PopulateWavetable()
{
    PopulateSawWT();
}

void SawWavetable::CreateHarmonics()
{
    for (int i=0; i<numSawHarmonics; ++i)
        sawHarmonics.add(new SinOsc());
}

void SawWavetable::SetSawSampleRates()
{
    for (int i=0; i<numSawHarmonics; ++i)
        sawHarmonics[i]->setSampleRate(sampleRate);
}

void SawWavetable::SetSawFrequencies()
{
    float harmonicFreq;
    
    for (int i=0; i<numSawHarmonics; ++i)
    {
        if (i == 0)
            harmonicFreq = frequency; // Fundamental 1/1 ratio
        else
            harmonicFreq *= ( (i + 1.0f) / i ); // partials up the harmonic series ratios

        sawHarmonics[i]->setFrequency(harmonicFreq);
    }
}

void SawWavetable::SumHarmonics()
{
    for (int i=0; i<waveTableSize; ++i)
    {
        for (int j=0; j<numSawHarmonics; ++j)
        {
            float harmonicAmplitude = 1.0f / (j + 1.0f);    // Amplitude of harmonic is 1/n where n is the harmonic number
            waveTable[i] += sawHarmonics[j]-> process() * harmonicAmplitude;
        }
    }
}

void SawWavetable::PopulateSawWT()
{
    CreateHarmonics();
    SetSawSampleRates();
    SetSawFrequencies();
    SumHarmonics();
    NormalizeWaveTable();
}

//=== SquareWavetable Class ================================================

SquareWavetable::SquareWavetable()
: numSquareHarmonics(57)    // Fundamental + 56 partials -- Adjust this number to mod square timbre
{}

SquareWavetable::~SquareWavetable()
{
    for (int i=0; i<numSquareHarmonics; ++i)
        squareHarmonics.remove(i);
}

void SquareWavetable::PopulateWavetable()
{
    PopulateSquareWT();
}

void SquareWavetable::CreateHarmonics()
{
    for (int i=0; i<numSquareHarmonics; ++i)
        squareHarmonics.add(new SinOsc());
}

void SquareWavetable::SetSquareSampleRates()
{
    for (int i=0; i<numSquareHarmonics; ++i)
        squareHarmonics[i]->setSampleRate(sampleRate);
}

void SquareWavetable::SetSquareFrequencies()
{
    int    twiceOddHarmonics = numSquareHarmonics * 2;
    float *harmonicFreq      = new float[twiceOddHarmonics];
    
    for (int i=0; i<twiceOddHarmonics; i++)
    {
        if (i == 0)
            harmonicFreq[i] = frequency; // Fundamental 1/1
        else            // Harmonic series ratios
            harmonicFreq[i] = harmonicFreq[i-1] * ((i + 1.0f) / i);
        
        if (i % 2 == 0)  // Use the odd harmonics (at the even indexes) to set the frequencies
            squareHarmonics[i / 2]->setFrequency(harmonicFreq[i]);
    }
    
    delete[] harmonicFreq;
}

void SquareWavetable::SumHarmonics()
{
    for (int i=0; i<waveTableSize; ++i)
    {
        for (int j=0; j<numSquareHarmonics; ++j)
        {
            float harmonicAmplitude = 1.0f / (j * 2.0f + 1.0f);    // Amplitude of harmonic is 1/n where n is the harmonic number
            waveTable[i] += squareHarmonics[j]-> process() * harmonicAmplitude;
        }
    }
}

void SquareWavetable::PopulateSquareWT()
{
    CreateHarmonics();
    SetSquareSampleRates();
    SetSquareFrequencies();
    SumHarmonics();
    NormalizeWaveTable();
}

//=== SpikeWavetable Class ================================================

void SpikeWavetable::HighPassSpike()
{
    cutoffFreq = frequency * 10.0f;
    
    highPass.reset();
    highPass.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, cutoffFreq, 2.0f));
    
    for (int i=0; i < waveTableSize; ++i)
        waveTable[i] = highPass.processSingleSampleRaw(waveTable[i]);
}

void SpikeWavetable::PopulateSquareWT()
{
    CreateHarmonics();
    SetSquareSampleRates();
    SetSquareFrequencies();
    SumHarmonics();
    HighPassSpike();
    NormalizeWaveTable();
}

