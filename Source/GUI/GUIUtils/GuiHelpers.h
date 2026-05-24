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
    // Styles a slider with our textbox layout (right of horizontal, below others).
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

        // 48px right fits "100 ms"-"999 ms"; 56px below fits "-XX.X dB".
        const int tbW = (tbPos == juce::Slider::TextBoxRight) ? 48 : 56;
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

    inline void SetupLabel(juce::Component       *parent
                           , juce::Label         &label
                           , juce::String         labelText
                           , juce::Colour         color
                           , float                fontSize
                           , juce::Justification  just = juce::Justification::centred)
    {
        label.setFont             (juce::FontOptions("helvetica", fontSize, 1));
        label.setText             (labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour           (juce::Label::textColourId, color);
        label.setJustificationType(just);

        parent->addAndMakeVisible(label);
    }

    // Small uppercase letter-spaced section header.
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

    inline void SetupComboBox(juce::Component     *parent
                              , juce::ComboBox    &box
                              , juce::StringArray  items)
    {
        box.addItemList         (items, 1);
        box.setJustificationType(juce::Justification::centred);
        box.setSelectedItemIndex(0);

        parent->addAndMakeVisible(box);
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

    // Single chokepoint for tooltip text so we can grep / gate / transform later.
    inline void SetTip(juce::SettableTooltipClient &control, const juce::String &text)
    {
        control.setTooltip(text);
    }
}
