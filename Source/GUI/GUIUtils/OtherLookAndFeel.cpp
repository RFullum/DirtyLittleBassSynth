/*
  ==============================================================================

    OtherLookAndFeel.cpp

  ==============================================================================
*/

#include "OtherLookAndFeel.h"
#include <cmath>

//============================================================

OtherLookAndFeel::OtherLookAndFeel()
: trackBackground(juce::Colour((juce::uint8)30, (juce::uint8)33, (juce::uint8)48))
{}

//============================================================

void OtherLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height
                                        , float sliderPos, float rotaryStartAngle, float rotaryEndAngle
                                        , juce::Slider &slider)
{
    const float boundsSize = (float)juce::jmin(width, height);
    const float cx         = (float)x + (float)width  * 0.5f;
    const float cy         = (float)y + (float)height * 0.5f;

    const float arcRadius  = boundsSize * 0.42f;
    const float arcThick   = juce::jmax(2.0f, boundsSize * 0.06f);

    const float curAngle   = juce::jmap(sliderPos, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);

    const auto fillColor   = slider.findColour(juce::Slider::rotarySliderFillColourId);

    // Background arc
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(trackBackground);
    g.strokePath(bgArc, juce::PathStrokeType(arcThick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, curAngle, true);
    g.setColour(fillColor);
    g.strokePath(valueArc, juce::PathStrokeType(arcThick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

//============================================================

/// Thin-track linear slider with a small round thumb. Filled portion uses the slider's
/// trackColourId; thumb uses thumbColourId; unfilled track uses trackBackground.
void OtherLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height
                                        , float sliderPos, float minSliderPos, float maxSliderPos
                                        , const juce::Slider::SliderStyle style, juce::Slider &slider)
{
    juce::ignoreUnused(minSliderPos, maxSliderPos);

    constexpr float trackThick  = 3.0f;
    constexpr float thumbRadius = 4.0f;

    const auto fillColor  = slider.findColour(juce::Slider::trackColourId);
    const auto thumbColor = slider.findColour(juce::Slider::thumbColourId);

    if (style == juce::Slider::SliderStyle::LinearHorizontal)
    {
        const float trackY = (float)y + (float)height * 0.5f - trackThick * 0.5f;
        const float trackX = (float)x;
        const float trackW = (float)width;

        g.setColour(trackBackground);
        g.fillRoundedRectangle(trackX, trackY, trackW, trackThick, trackThick * 0.5f);

        g.setColour(fillColor);
        g.fillRoundedRectangle(trackX, trackY, sliderPos - trackX, trackThick, trackThick * 0.5f);

        g.setColour(thumbColor);
        g.fillEllipse(sliderPos - thumbRadius
                      , (float)y + (float)height * 0.5f - thumbRadius
                      , thumbRadius * 2.0f
                      , thumbRadius * 2.0f);
    }
    else if (style == juce::Slider::SliderStyle::LinearVertical)
    {
        const float trackX = (float)x + (float)width * 0.5f - trackThick * 0.5f;
        const float trackY = (float)y;
        const float trackH = (float)height;

        g.setColour(trackBackground);
        g.fillRoundedRectangle(trackX, trackY, trackThick, trackH, trackThick * 0.5f);

        // Fill from bottom (max y) up to thumb position (sliderPos has lower y for higher value).
        g.setColour(fillColor);
        g.fillRoundedRectangle(trackX, sliderPos, trackThick,
                               (float)(y + height) - sliderPos, trackThick * 0.5f);

        g.setColour(thumbColor);
        g.fillEllipse((float)x + (float)width * 0.5f - thumbRadius
                      , sliderPos - thumbRadius
                      , thumbRadius * 2.0f
                      , thumbRadius * 2.0f);
    }
    else
    {
        juce::LookAndFeel_V4::drawLinearSlider(g
                                               , x
                                               , y
                                               , width
                                               , height
                                               , sliderPos
                                               , minSliderPos
                                               , maxSliderPos
                                               , style
                                               , slider);
    }
}

//============================================================

void OtherLookAndFeel::SetTrackBackground(juce::Colour color)
{
    trackBackground = color;
}
