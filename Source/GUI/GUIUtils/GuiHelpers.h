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

    // Attach* helpers tag the component with "paramID" so MidiLearnOverlay
    // can resolve a click to its bound APVTS parameter. Always use these for
    // learnable controls.

    inline std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    AttachSlider(juce::AudioProcessorValueTreeState &apvts,
                 const juce::String                 &paramID,
                 juce::Slider                       &slider)
    {
        slider.getProperties().set("paramID", paramID);
        return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramID, slider);
    }

    inline std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
    AttachButton(juce::AudioProcessorValueTreeState &apvts,
                 const juce::String                 &paramID,
                 juce::Button                       &button)
    {
        button.getProperties().set("paramID", paramID);
        return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramID, button);
    }

}
