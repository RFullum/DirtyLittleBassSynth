/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 27 Dec 2020 6:36:38pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    /// Constructor
    OtherLookAndFeel();
    
    /// Sets the dial and tick colors
    void setColors(juce::Colour dial, juce::Colour tick, juce::Colour back);
    
    /// Draws rotary slider as round knob
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    
    /// Draws linear slider with a square thumb, only as wide as the track
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
private:
    juce::Colour dialColor;
    juce::Colour tickColor;
    juce::Colour backColor;
};
