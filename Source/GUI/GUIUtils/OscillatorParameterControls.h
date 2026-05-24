/*
  ==============================================================================

    OscillatorParameterControls.h
    Created: 28 Apr 2020 4:17:05pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

// Maps a 0..2 morph parameter onto per-shape gains.
//   val 0 -> sin/sub-sin/etc. = 1, others = 0
//   val 1 -> middle shape    = 1, others = 0
//   val 2 -> saw             = 1, others = 0
class OscParamControl
{
public:
    OscParamControl();

    float SinMorphGain  (std::atomic<float>* oscMorphVal);
    float SpikeMorphGain(std::atomic<float>* oscMorphVal);
    float SawMorphGain  (std::atomic<float>* oscMorphVal);

protected:
    // 1.0 = linear blend; >1 = less overlap; 0.1..1 = more overlap.
    float blendCurve = 0.2f;

private:
    float LevelFormula(std::atomic<float>* MV, float CV);
};


//==============================================================================

class SubOscParamControl
    : public OscParamControl
{
public:
    float SinSubGain   (std::atomic<float>* subMorphVal);
    float SquareSubGain(std::atomic<float>* subMorphVal);
    float SawSubGain   (std::atomic<float>* subMorphVal);

    // Returns octave denominator: 1, 2, or 4.
    int SubOctaveSelector(std::atomic<float>* subOctVal);
};
