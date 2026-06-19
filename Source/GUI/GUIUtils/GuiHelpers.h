/*
  ==============================================================================

    GuiHelpers.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

namespace DLBS
{

    // Cutoff slider position (1..100) -> Hz/kHz. Display only — mirrors
    // FilterModulation::KeyMapFixed (20Hz..17kHz).
    inline juce::String FormatCutoffHz(double pos)
    {
        const float hz = juce::jmap((float)pos, 1.0f, 100.0f, 20.0f, 17000.0f);

        if (hz < 1000.0f)
            return juce::String((int)std::round(hz)) + " Hz";

        return juce::String(hz / 1000.0f, 2) + " kHz";
    }

}
