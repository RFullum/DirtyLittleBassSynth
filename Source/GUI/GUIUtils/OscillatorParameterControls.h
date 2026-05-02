/*
  ==============================================================================

    OscillatorParameterControls.h
    Created: 28 Apr 2020 4:17:05pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//=================================================================

/// Maps the main oscillator morph parameter (0..2) to per-shape gain levels.
class OscParamControl
{
public:
    OscParamControl();

    /// val 0 = amp 1; val 1 = amp 0; val 2 = amp 0
    float SinMorphGain(std::atomic<float>* oscMorphVal);

    /// val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
    float SpikeMorphGain(std::atomic<float>* oscMorphVal);

    /// val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
    float SawMorphGain(std::atomic<float>* oscMorphVal);

protected:
    float blendCurve;

private:
    /// Maps parameter value to gain. Values 0..2 map to amplitudes 0..1.
    float LevelFormula(std::atomic<float>* MV, float CV);
};


//=================================================================

/// Adds sub-octave selection on top of OscParamControl.
class SubOscParamControl
    : public OscParamControl
{
public:
    /// val 0 = amp 1; val 1 = amp 0; val 2 = amp 0
    float SinSubGain(std::atomic<float>* subMorphVal);

    /// val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
    float SquareSubGain(std::atomic<float>* subMorphVal);

    /// val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
    float SawSubGain(std::atomic<float>* subMorphVal);

    /// Returns octave denominator: 1, 2, or 4.
    int SubOctaveSelector(std::atomic<float>* subOctVal);
};
