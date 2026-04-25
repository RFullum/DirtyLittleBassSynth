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
    float sinMorphGain(std::atomic<float>* oscMorphVal);

    /// val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
    float spikeMorphGain(std::atomic<float>* oscMorphVal);

    /// val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
    float sawMorphGain(std::atomic<float>* oscMorphVal);

protected:
    float blendCurve;

private:
    /// Maps parameter value to gain. Values 0..2 map to amplitudes 0..1.
    float levelFormula(std::atomic<float>* MV, float CV);
};


//=================================================================

/// Adds sub-octave selection on top of OscParamControl.
class SubOscParamControl : public OscParamControl
{
public:
    /// val 0 = amp 1; val 1 = amp 0; val 2 = amp 0
    float sinSubGain(std::atomic<float>* subMorphVal);

    /// val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
    float squareSubGain(std::atomic<float>* subMorphVal);

    /// val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
    float sawSubGain(std::atomic<float>* subMorphVal);

    /// Returns octave denominator: 1, 2, or 4.
    int subOctaveSelector(std::atomic<float>* subOctVal);
};
