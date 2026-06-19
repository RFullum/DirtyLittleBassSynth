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

class SubOscParamControl
{
public:
    // Returns octave denominator: 1, 2, or 4.
    int SubOctaveSelector(std::atomic<float>* subOctVal);
};
