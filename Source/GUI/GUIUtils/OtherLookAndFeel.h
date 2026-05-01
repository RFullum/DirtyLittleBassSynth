/*
  ==============================================================================

    OtherLookAndFeel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//============================================================

class OtherLookAndFeel
    : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel();

    /// Sets the dim background color used for unfilled portions of tracks and arcs.
    /// Per-control accent / thumb colors come from the slider's own colour ids.
    void setTrackBackground(juce::Colour color);

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height
                          , float sliderPos, float rotaryStartAngle, float rotaryEndAngle
                          , juce::Slider &slider) override;

    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height
                          , float sliderPos, float minSliderPos, float maxSliderPos
                          , const juce::Slider::SliderStyle style, juce::Slider &slider) override;

private:
    juce::Colour trackBackground;
};
