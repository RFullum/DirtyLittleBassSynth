/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 27 Dec 2020 6:36:38pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    /// Constructor
    OtherLookAndFeel();
    
    void drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, Slider &slider) override;
    
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const Slider::SliderStyle style, Slider& slider) override;
    
};
