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
    /// Configures a slider's style, colors, optional textbox; adds to parent.
    inline void SetupSlider(juce::Component             *parent
                            , juce::Slider              &slider
                            , juce::Slider::SliderStyle  style
                            , juce::Colour               fillColor
                            , juce::Colour               thumbColor
                            , juce::Colour               textColor
                            , bool                       showTextBox)
    {
        slider.setSliderStyle(style);

        if (showTextBox)
        {
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
            slider.setColour      (juce::Slider::textBoxOutlineColourId, juce::Colour((juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0));
            slider.setColour      (juce::Slider::textBoxTextColourId,    textColor);
        }
        else
        {
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        }

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

    /// Configures a combobox's items; adds to parent.
    inline void SetupComboBox(juce::Component    *parent
                              , juce::ComboBox   &box
                              , juce::StringArray items)
    {
        box.addItemList         (items, 1);
        box.setJustificationType(juce::Justification::centred);
        box.setSelectedItemIndex(0);

        parent->addAndMakeVisible(box);
    }
}
