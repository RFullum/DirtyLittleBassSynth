/*
  ==============================================================================

    OscillatorParameterControls.cpp
    Created: 26 Dec 2020 12:47:03pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OscillatorParameterControls.h"

//==============================================================================

int SubOscParamControl::SubOctaveSelector(std::atomic<float>* subOctVal)
{
    if (*subOctVal == 1.0f) return 2;
    if (*subOctVal == 2.0f) return 4;
    return 1;
}
