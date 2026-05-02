/*
  ==============================================================================

    GuiHelpers.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//============================================================

namespace dlbs
{
    /// Configures a slider's style, colors, and an always-visible value text box.
    /// Text box is placed to the right of horizontal sliders, below vertical and rotary.
    inline void SetupSlider(juce::Component             *parent
                            , juce::Slider              &slider
                            , juce::Slider::SliderStyle  style
                            , juce::Colour               fillColor
                            , juce::Colour               thumbColor
                            , juce::Colour               textColor)
    {
        slider.setSliderStyle(style);

        const auto tbPos = (style == juce::Slider::SliderStyle::LinearHorizontal)
                              ? juce::Slider::TextBoxRight
                              : juce::Slider::TextBoxBelow;

        const int tbW = (tbPos == juce::Slider::TextBoxRight) ? 38 : 44;
        const int tbH = 14;

        slider.setTextBoxStyle(tbPos, false, tbW, tbH);
        slider.setColour      (juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
        slider.setColour      (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        slider.setColour      (juce::Slider::textBoxTextColourId,       textColor);

        if (style == juce::Slider::SliderStyle::LinearHorizontal
            || style == juce::Slider::SliderStyle::LinearVertical)
            slider.setColour(juce::Slider::trackColourId, fillColor);
        else if (style == juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag)
            slider.setColour(juce::Slider::rotarySliderFillColourId, fillColor);

        slider.setColour(juce::Slider::thumbColourId, thumbColor);

        parent->addAndMakeVisible(slider);
    }

    /// Configures a label's font/text/color; adds to parent.
    inline void SetupLabel(juce::Component  *parent
                           , juce::Label    &label
                           , juce::String    labelText
                           , juce::Colour    color
                           , float           fontSize)
    {
        label.setFont             (juce::FontOptions("helvetica", fontSize, 1));
        label.setText             (labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour           (juce::Label::textColourId, color);

        parent->addAndMakeVisible(label);
    }

    /// Configures a small uppercase letter-spaced section header label and adds to parent.
    inline void SetupSectionLabel(juce::Component *parent
                                  , juce::Label   &label
                                  , juce::String   text
                                  , juce::Colour   color)
    {
        auto font = juce::Font(juce::FontOptions("helvetica", 10.0f, juce::Font::bold))
                       .withExtraKerningFactor(0.18f);

        label.setFont             (font);
        label.setText             (text.toUpperCase(), juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centredLeft);
        label.setColour           (juce::Label::textColourId, color);

        parent->addAndMakeVisible(label);
    }

    /// Formats a slider value as ms (under 1s) or s (>= 1s), useful for ADSR times.
    inline juce::String FormatTime(double seconds)
    {
        if (seconds < 1.0)
            return juce::String((int)std::round(seconds * 1000.0)) + " ms";

        return juce::String(seconds, 2) + " s";
    }

    /// Formats a linear gain (0..N) as dB, with -inf for near-silence.
    inline juce::String FormatGainDb(double linear)
    {
        if (linear <= 0.0001)
            return juce::String::fromUTF8("\xe2\x88\x92\xe2\x88\x9e dB");

        const auto db = juce::Decibels::gainToDecibels((float)linear);
        return juce::String(db, 1) + " dB";
    }

    /// Formats a 0..1 value as a percent.
    inline juce::String FormatPercent(double v)
    {
        return juce::String((int)std::round(v * 100.0)) + "%";
    }

    /// Maps the cutoff slider position (1..100, log-tapered) onto Hz/kHz for display.
    /// Mirrors FilterModulation::KeyMapFixed (20Hz..17kHz) — display only.
    inline juce::String FormatCutoffHz(double pos)
    {
        const float hz = juce::jmap((float)pos, 1.0f, 100.0f, 20.0f, 17000.0f);

        if (hz < 1000.0f)
            return juce::String((int)std::round(hz)) + " Hz";

        return juce::String(hz / 1000.0f, 2) + " kHz";
    }

    /// Installs a value-display formatter on a slider. The reverse parser strips
    /// non-numeric trailing text so users can type either "1.2" or "1.2 s".
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

    /// Configures a combobox's items; adds to parent.
    inline void SetupComboBox(juce::Component     *parent
                              , juce::ComboBox    &box
                              , juce::StringArray  items)
    {
        box.addItemList         (items, 1);
        box.setJustificationType(juce::Justification::centred);
        box.setSelectedItemIndex(0);

        parent->addAndMakeVisible(box);
    }
}
