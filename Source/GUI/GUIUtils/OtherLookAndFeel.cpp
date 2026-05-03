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

    // "snapAt50" property: small radial tick at the centre of the rotary range.
    // Brightens to full thumb-colour when within 1.5% of dead centre. No actual
    // snapping behaviour — purely a visual cue for finding 50% (e.g. on dry/wet
    // controls where 50/50 is a meaningful balance point).
    const bool snapAt50 = (bool) slider.getProperties().getWithDefault("snapAt50", false);

    if (snapAt50)
    {
        const float centreAngle = juce::jmap(0.5f, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
        const bool  atCentre    = std::abs(sliderPos - 0.5f) < 0.015f;

        const auto  thumbColor  = slider.findColour(juce::Slider::thumbColourId);
        g.setColour(atCentre ? thumbColor : thumbColor.withAlpha(0.35f));

        const float r1 = arcRadius - arcThick * 0.5f - 1.0f;
        const float r2 = arcRadius + arcThick * 0.5f + 1.0f;

        const float x1 = cx + std::sin(centreAngle) * r1;
        const float y1 = cy - std::cos(centreAngle) * r1;
        const float x2 = cx + std::sin(centreAngle) * r2;
        const float y2 = cy - std::cos(centreAngle) * r2;

        g.drawLine(x1, y1, x2, y2, 1.5f);
    }
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

    const bool snapAt50 = (bool) slider.getProperties().getWithDefault("snapAt50", false);

    if (style == juce::Slider::SliderStyle::LinearHorizontal)
    {
        const float trackY = (float)y + (float)height * 0.5f - trackThick * 0.5f;
        const float trackX = (float)x;
        const float trackW = (float)width;

        g.setColour(trackBackground);
        g.fillRoundedRectangle(trackX, trackY, trackW, trackThick, trackThick * 0.5f);

        // "bipolarFill" property: fill spans from the track centre to the thumb,
        // not from the left edge. Used for bipolar controls like the stereo widener.
        const bool bipolar = (bool) slider.getProperties().getWithDefault("bipolarFill", false);

        g.setColour(fillColor);

        if (bipolar)
        {
            const float centerX = trackX + trackW * 0.5f;
            const float fillX   = juce::jmin(centerX, sliderPos);
            const float fillW   = std::abs(sliderPos - centerX);

            g.fillRoundedRectangle(fillX, trackY, fillW, trackThick, trackThick * 0.5f);
        }
        else
        {
            g.fillRoundedRectangle(trackX, trackY, sliderPos - trackX, trackThick, trackThick * 0.5f);
        }

        g.setColour(thumbColor);
        g.fillEllipse(sliderPos - thumbRadius
                      , (float)y + (float)height * 0.5f - thumbRadius
                      , thumbRadius * 2.0f
                      , thumbRadius * 2.0f);

        // 50% snap-marker tick (above + below the track at the centre x).
        if (snapAt50)
        {
            const float norm     = (sliderPos - trackX) / juce::jmax(1.0f, trackW);
            const bool  atCentre = std::abs(norm - 0.5f) < 0.015f;
            const float tickX    = trackX + trackW * 0.5f;

            g.setColour(atCentre ? thumbColor : thumbColor.withAlpha(0.35f));
            g.drawLine(tickX, trackY - 2.0f,
                       tickX, trackY + trackThick + 2.0f, 1.5f);
        }
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

        // 50% snap-marker tick (left + right of the track at the centre y).
        // For vertical sliders, lower y is higher value, so invert when computing
        // the normalised position from the thumb's pixel coordinate.
        if (snapAt50)
        {
            const float norm     = 1.0f - (sliderPos - trackY) / juce::jmax(1.0f, trackH);
            const bool  atCentre = std::abs(norm - 0.5f) < 0.015f;
            const float tickY    = trackY + trackH * 0.5f;

            g.setColour(atCentre ? thumbColor : thumbColor.withAlpha(0.35f));
            g.drawLine(trackX - 2.0f,                  tickY,
                       trackX + trackThick + 2.0f,     tickY, 1.5f);
        }
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
