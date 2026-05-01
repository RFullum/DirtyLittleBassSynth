/*
  ==============================================================================

    GuiResources.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

//============================================================

/// Editor-owned resources shared across all panels: the parameter tree, look-and-feels,
/// and active theme. Each panel takes a non-const ref to one of these in its constructor.
struct GuiResources
{
    juce::AudioProcessorValueTreeState *apvts;
    juce::LookAndFeel                  *dialLookAndFeel;
    juce::LookAndFeel                  *dryWetLookAndFeel;
    const Palette::Theme               &theme;
};
