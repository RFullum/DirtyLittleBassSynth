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


    // Formats as "X ms" under 1s, "X.XX s" otherwise. Used by ADSR sliders.
    inline juce::String FormatTime(double seconds)
    {
        if (seconds < 1.0)
            return juce::String((int)std::round(seconds * 1000.0)) + " ms";

        return juce::String(seconds, 2) + " s";
    }

    // Linear gain -> dB, with "-inf" for near-silence.
    inline juce::String FormatGainDb(double linear)
    {
        if (linear <= 0.0001)
            return juce::String::fromUTF8("\xe2\x88\x92\xe2\x88\x9e dB");

        const auto db = juce::Decibels::gainToDecibels((float)linear);
        return juce::String(db, 1) + " dB";
    }

    inline juce::String FormatPercent(double v)
    {
        return juce::String((int)std::round(v * 100.0)) + "%";
    }

    // Cutoff slider position (1..100) -> Hz/kHz. Display only — mirrors
    // FilterModulation::KeyMapFixed (20Hz..17kHz).
    inline juce::String FormatCutoffHz(double pos)
    {
        const float hz = juce::jmap((float)pos, 1.0f, 100.0f, 20.0f, 17000.0f);

        if (hz < 1000.0f)
            return juce::String((int)std::round(hz)) + " Hz";

        return juce::String(hz / 1000.0f, 2) + " kHz";
    }

    // Reverse parser strips non-numeric trailing text so users can type "1.2" or "1.2 s".
    inline void SetSliderTextFormat(juce::Slider                          &slider
                                    , std::function<juce::String(double)> formatter)
    {
        slider.textFromValueFunction = std::move(formatter);
        slider.valueFromTextFunction = [](const juce::String &text)
        {
            return text.retainCharacters("0123456789.-").getDoubleValue();
        };
        slider.updateText();
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
