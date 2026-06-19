/*
  ==============================================================================

    FilterModulation.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utils/Mapping.h"

//======================================================

struct FilterModulation
{
    float maxCutoff    = 18000.0f;
    float minCutoff    = 20.0f;
    float maxResonance = 3.0f;

    float cutoffFreq     = 1000.0f;
    float resonance      = 1.0f;
    float cutoffLFO      = 1000.0f;   // final modulated cutoff (Hz)
    float resonanceScale = 0.1f;      // final modulated resonance

    // Key-tracked position→freq: floor is two octaves below the note, ceiling
    // is maxCutoff, exponential between (ExpMap clamps cutoffPos internally).
    void KeyMapTracked(float noteFreq, float cutoffPos)
    {
        const float floor = juce::jmax(minCutoff, noteFreq * 0.125f);
        cutoffFreq = FullumDspUtils::ExpMap(cutoffPos, floor, maxCutoff);
    }

    // Env→cutoff/res + LFO onto cutoff. Verbatim from the old FilterSection.cpp.
    void ApplyEnvAndLfo(float envVal, float amtToCO, float amtToRes, float lfoVal, float amtToLFO)
    {
        float filterHeadroom = (maxCutoff - cutoffFreq) * amtToCO;
        float cutoffScale    = juce::jmap(envVal, 0.0f, 1.0f, cutoffFreq, cutoffFreq + filterHeadroom);
        if (cutoffScale <= 0.0f)
            cutoffScale = 0.01f;

        float resHeadroom = (maxResonance - resonance) * amtToRes;
        resonanceScale    = juce::jmap(envVal, 0.1f, 0.98f, resonance, resonance + resHeadroom);

        float headroom  = (maxCutoff - cutoffScale) * amtToLFO;
        float floorroom = (cutoffScale - minCutoff) * amtToLFO;

        if (lfoVal >= 0.0f)
            cutoffLFO = cutoffScale + (lfoVal * headroom);
        else
            cutoffLFO = cutoffScale + (lfoVal * floorroom);
    }
};
