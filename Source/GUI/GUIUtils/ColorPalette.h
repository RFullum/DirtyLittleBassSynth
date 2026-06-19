/*
  ==============================================================================

    ColorPalette.h
    Created: 27 Apr 2026 11:39:53am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GUI/ColorPalette.h"
#include <JuceHeader.h>

//==============================================================================

namespace Palette
{

    inline const Theme DefaultTheme
    {
        .background      = juce::Colour(0xFF0D0E12),   // not-quite-black
        .structure       = juce::Colour(0xFF1A1C24),   // dark blue-grey
        .primaryAccent   = juce::Colour(0xFF00C8B4),   // teal
        .secondaryAccent = juce::Colour(0xFF4C6EF5),   // purplish
        .textPrimary     = juce::Colour(0xFFD0D8E8),   // blue-tinted off-white
        .textSecondary   = juce::Colour(0xFF5A6480),   // blue-gray
        .pinkAccent      = juce::Colour(0xFFFF2D78),   // hot pink
        .orangeAccent    = juce::Colour(0xFFFF6B00)    // blaze orange
    };

}   // namespace Palette
