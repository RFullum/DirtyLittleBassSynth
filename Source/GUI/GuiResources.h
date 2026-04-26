/*
  ==============================================================================

    GuiResources.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//============================================================

/// Editor-owned resources shared across all panels: the parameter tree, look-and-feels,
/// and palette. Each panel takes a non-const ref to one of these in its constructor.
struct GuiResources
{
    juce::AudioProcessorValueTreeState *apvts             = nullptr;
    juce::LookAndFeel                  *dialLookAndFeel   = nullptr;
    juce::LookAndFeel                  *dryWetLookAndFeel = nullptr;

    juce::Colour onyx;
    juce::Colour lightSlateGray;
    juce::Colour magicMint;
    juce::Colour fieryRose;
    juce::Colour orangePeel;
    juce::Colour textColor;
};
