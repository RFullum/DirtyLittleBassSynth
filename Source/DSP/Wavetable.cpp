/*
  ==============================================================================

    Wavetable.cpp
    Created: 26 Dec 2020 3:25:26pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Wavetable.h"
#include <JuceHeader.h>
#include <cmath>

//=== Wavetable Class: Sine Wave =================================

Wavetable::Wavetable()
: sampleRate(44100.0f)
, readHeadPos(0.0f)
, increment(1.0f)
, levelLow(0)
, levelHigh(0)
, blendT(0.0f)
{}

void Wavetable::SetSampleRate(float SR)
{
    sampleRate = SR;
}

void Wavetable::PopulateWavetable()
{
    const int levels = NumLevels();

    waveTable.assign(levels, std::vector<float>(waveTableSize, 0.0f));
    sinOsc.setSampleRate(sampleRate);

    for (int level = 0; level < levels; ++level)
    {
        BuildLevel(level);
        NormalizeLevel(level);
    }
}

void Wavetable::BuildLevel(int level)
{
    sinOsc.setFrequency(sampleRate / (float) waveTableSize);

    auto &table = waveTable[level];
    for (int i = 0; i < waveTableSize; ++i)
        table[i] = sinOsc.process();
}

float Wavetable::Process()
{
    const float low  = SampleAt(levelLow,  readHeadPos);
    const float high = SampleAt(levelHigh, readHeadPos);
    const float out  = low + (high - low) * blendT;

    readHeadPos += increment;
    if (readHeadPos >= waveTableSize)
        readHeadPos -= waveTableSize;

    return out;
}

void Wavetable::SetIncrement(float noteFreq)
{
    increment = (noteFreq * waveTableSize) / sampleRate;

    if (NumLevels() <= 1)
    {
        levelLow  = 0;
        levelHigh = 0;
        blendT    = 0.0f;
        return;
    }

    // Position within the mipmap stack: 0 = lowestFundamental, 1 = one octave up, etc.
    float octavePos = std::log2(noteFreq / lowestFundamental);
    if (octavePos < 0.0f)
        octavePos = 0.0f;

    const int maxLevel = NumLevels() - 1;
    if (octavePos > (float) maxLevel)
        octavePos = (float) maxLevel;

    levelLow  = (int) std::floor(octavePos);
    levelHigh = juce::jmin(levelLow + 1, maxLevel);
    blendT    = octavePos - (float) levelLow;
}

float Wavetable::GetWavetableSampleValue(int index)
{
    return waveTable[0][index];
}

int Wavetable::HarmonicCapForLevel(int level) const
{
    // Top of this level's octave; harmonics whose frequency at that fundamental
    // would exceed Nyquist must be excluded.
    const float topFundamental = lowestFundamental * (float) (1 << (level + 1));
    const float nyquist        = sampleRate * 0.5f;

    return (int) std::floor(nyquist / topFundamental);
}

void Wavetable::NormalizeLevel(int level)
{
    auto &table = waveTable[level];

    float peak = 0.0f;
    for (float s : table)
        peak = juce::jmax(peak, std::abs(s));

    if (peak > 0.0f)
        for (float &s : table)
            s /= peak;
}

float Wavetable::SampleAt(int level, float readPos) const
{
    const auto &table = waveTable[level];
    const float frac  = readPos - std::floor(readPos);
    float       sum   = 0.0f;

    for (int i = -2; i < 2; ++i)
    {
        int idx = (int) std::floor(readPos + (i + 1));
        idx %= waveTableSize;
        if (idx < 0)
            idx += waveTableSize;

        float outVal      = table[idx];
        float denominator = 1.0f;

        for (int j = -2; j < 2; ++j)
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


//=== SawWavetable ================================================

void SawWavetable::BuildLevel(int level)
{
    const int harmonicCap = HarmonicCapForLevel(level);
    const int numSummed   = juce::jmin(maxHarmonics, harmonicCap);
    const float fundamental = sampleRate / (float) waveTableSize;

    juce::OwnedArray<SinOsc> oscs;
    for (int h = 0; h < numSummed; ++h)
    {
        auto *osc = new SinOsc();
        osc->setSampleRate(sampleRate);
        osc->setFrequency(fundamental * (float) (h + 1));   // harmonics 1, 2, 3, ...
        oscs.add(osc);
    }

    auto &table = waveTable[level];
    for (int i = 0; i < waveTableSize; ++i)
    {
        for (int h = 0; h < numSummed; ++h)
        {
            const float amp = 1.0f / (float) (h + 1);       // 1/n rolloff
            table[i] += oscs[h]->process() * amp;
        }
    }
}


//=== SquareWavetable ================================================

void SquareWavetable::BuildLevel(int level)
{
    const int harmonicCap = HarmonicCapForLevel(level);
    // Square uses odd harmonics 1, 3, 5, ...; the (n)th odd harmonic = 2n-1.
    const int aliasOddCap = (harmonicCap + 1) / 2;
    const int numSummed   = juce::jmin(maxHarmonics, aliasOddCap);
    const float fundamental = sampleRate / (float) waveTableSize;

    juce::OwnedArray<SinOsc> oscs;
    for (int n = 0; n < numSummed; ++n)
    {
        const int harmonicIndex = 2 * n + 1;                // 1, 3, 5, ...
        auto     *osc           = new SinOsc();
        osc->setSampleRate(sampleRate);
        osc->setFrequency(fundamental * (float) harmonicIndex);
        oscs.add(osc);
    }

    auto &table = waveTable[level];
    for (int i = 0; i < waveTableSize; ++i)
    {
        for (int n = 0; n < numSummed; ++n)
        {
            const float amp = 1.0f / (float) (2 * n + 1);   // 1/(2n+1)
            table[i] += oscs[n]->process() * amp;
        }
    }
}


//=== SpikeWavetable ================================================

void SpikeWavetable::BuildLevel(int level)
{
    SquareWavetable::BuildLevel(level);

    // High-pass each level. Cutoff is a fixed multiple of the wavetable's
    // generation fundamental; same shape per level, applied to that level's content.
    const float fundamental = sampleRate / (float) waveTableSize;
    const float cutoffFreq  = fundamental * 10.0f;

    juce::IIRFilter highPass;
    highPass.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, cutoffFreq, 2.0f));

    auto &table = waveTable[level];
    for (int i = 0; i < waveTableSize; ++i)
        table[i] = highPass.processSingleSampleRaw(table[i]);
}
