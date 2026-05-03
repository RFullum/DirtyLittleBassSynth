/*
  ==============================================================================

    Wavetable.h
    Created: 26 Apr 2020 2:38:54pm
    Author:  Robert Fullum

 Parent Class: Wavetable
 -Single sine cycle (no mipmap needed)

 Children:
 -SawWavetable     (mipmapped, one level per octave)
 -SquareWavetable  (mipmapped, one level per octave)

 Grandchildren:
 -SpikeWavetable (child of SquareWavetable, mipmapped)
  ==============================================================================
*/

#pragma once

#include <vector>
#include "Oscillators.h"

//=== Wavetable Class: Sine Wave =================================

class Wavetable
{
public:
    Wavetable();
    virtual ~Wavetable() = default;

    void  SetSampleRate(float SR);
    void  PopulateWavetable();
    float Process();
    void  SetIncrement(float noteFreq);
    float GetWavetableSampleValue(int index);   // returns level 0; for visualizer use

    /// Sets the read head to a normalised phase (0..1, wraps if out of range).
    /// Used by the filter-LFO sync stage to lock the LFO to host transport position.
    void  SetPhase(float phase01);

    static constexpr int   waveTableSize     = 1024;
    static constexpr int   numMipmapLevels   = 10;     // covers ~10 octaves from lowestFundamental
    static constexpr int   maxHarmonics      = 57;     // tonal cap; aliasing cap is computed per level
    static constexpr float lowestFundamental = 16.35f; // MIDI 12 (C0)

protected:
    /// Number of mipmap levels for this shape. Base (sine) returns 1.
    virtual int NumLevels() const { return 1; }

    /// Fills waveTable[level] with this shape's band-limited content for that level.
    /// Base implementation populates one sine cycle.
    virtual void BuildLevel(int level);

    /// Highest harmonic index that can be included at this mipmap level
    /// without exceeding Nyquist at the top of the level's octave.
    int HarmonicCapForLevel(int level) const;

    /// Rescale waveTable[level] to peak magnitude 1.0.
    void NormalizeLevel(int level);

    float sampleRate;

    // waveTable[mipmapLevel][sampleIndex].
    // Outer size is 1 for sine, numMipmapLevels for mipmapped shapes.
    std::vector<std::vector<float>> waveTable;

    SinOsc sinOsc;

private:
    /// Lagrange-interpolated read from one mipmap level.
    float SampleAt(int level, float readPos) const;

    float readHeadPos;
    float increment;

    // Mipmap selection: SetIncrement updates these, Process consumes them via lerp.
    int   levelLow;
    int   levelHigh;
    float blendT;
};


//========================================================================

class SawWavetable
    : public Wavetable
{
public:
    SawWavetable() = default;

protected:
    int  NumLevels() const override { return numMipmapLevels; }
    void BuildLevel(int level) override;
};


//========================================================================

class SquareWavetable
    : public Wavetable
{
public:
    SquareWavetable() = default;

protected:
    int  NumLevels() const override { return numMipmapLevels; }
    void BuildLevel(int level) override;
};


//========================================================================

class SpikeWavetable
    : public SquareWavetable
{
protected:
    void BuildLevel(int level) override;
};
