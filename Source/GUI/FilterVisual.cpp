/*
  ==============================================================================

    FilterVisual.cpp
    Created: 1 Jan 2021 6:08:24pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "FilterVisual.h"

//==============================================================================

FilterVisual::FilterVisual()
: lineColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
, bgColor  (juce::Colour((juce::uint8)7,   (juce::uint8)10, (juce::uint8)59))
, fadeColor(juce::Colour((juce::uint8)100, (juce::uint8)36, (juce::uint8)0))
{}

void FilterVisual::setColors(juce::Colour line, juce::Colour background, juce::Colour fade)
{
    lineColor = line;
    bgColor   = background;
    fadeColor = fade;
}

void FilterVisual::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    // Background fill
    g.setGradientFill     (juce::ColourGradient::vertical(bgColor, fadeColor, visualBox));
    g.fillRoundedRectangle(visualBox, cornerRound);

    // Center reference line
    const float cy = (float) getHeight() * 0.5f;
    g.setColour(lineColor.withAlpha(0.15f));
    g.drawLine(0.0f, cy, (float) getWidth(), cy, 0.5f);

    // Faint area fill below the curve
    g.setColour(lineColor.withAlpha(0.08f));
    g.fillPath(filterArea);

    // Filter response line
    g.setColour(lineColor);
    g.strokePath(filterShape,
                 juce::PathStrokeType(1.4f,
                                      juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));
}

void FilterVisual::resized()
{
    int  reducer     = 2;
    auto totalArea   = getLocalBounds();
    auto reducedArea = totalArea.reduced(reducer);

    visualBox.setBounds(reducedArea.getX(), reducedArea.getY(),
                        reducedArea.getWidth(), reducedArea.getHeight());
}

void FilterVisual::drawFilterShape(int filtType, float cutoff, float res)
{
    cutoffFreq = cutoff;
    resonance  = res;

    filterShape.clear();
    filterArea .clear();

    buildPaths(filtType);

    repaint();
}

void FilterVisual::buildPaths(int type)
{
    const float halfHeight = (float) getHeight() * 0.5f;
    const float resMap     = juce::jmap(resonance, 1.0f, 2.0f, 0.0f, halfHeight * 2.5f);

    // rolloffControl moves the cubic control points horizontally to shape the
    // steepness of the curve per filter order.
    float rolloffControl;
    switch (type)
    {
        case 1:  rolloffControl = 66.0f; break;     // -12dB LPF
        case 2:  rolloffControl = 33.0f; break;     // -24dB LPF
        case 3:  rolloffControl = 11.0f; break;     // -48dB LPF
        case 4:  rolloffControl = 33.0f; break;     // Notch
        default: rolloffControl = 66.0f; break;
    }

    constexpr float reducer = 2.0f;

    const float x1 = reducer;
    const float y1 = halfHeight;
    const float x2 = juce::jmap(powf(cutoffFreq * 0.01f, 0.25f),
                                0.32f, 1.0f,
                                15.0f, (float) getWidth() - reducer);
    const float y2 = halfHeight - resMap;
    const float bottomY = (float) getHeight() - reducer;
    const float rightX  = (float) getWidth()  - reducer;

    juce::Point<float> origin   (x1, y1);
    juce::Point<float> maxFilter(x2, bottomY);
    juce::Point<float> ctrlPt1  (juce::jmax(reducer, x2 - rolloffControl), halfHeight);
    juce::Point<float> ctrlPt2  (juce::jmax(reducer, x2 - rolloffControl), y2);

    // --- Line path: the response curve only (open, for stroking).
    filterShape.startNewSubPath(origin);
    filterShape.cubicTo        (ctrlPt1, ctrlPt2, maxFilter);

    if (type == 4)
    {
        // Notch climbs back up to the centerline at the right edge.
        juce::Point<float> maxFreq(rightX, halfHeight);
        juce::Point<float> ctrlPt3(juce::jmin(rightX, x2 + rolloffControl), y2);
        juce::Point<float> ctrlPt4(juce::jmin(rightX, x2 + rolloffControl), halfHeight);

        filterShape.cubicTo(ctrlPt3, ctrlPt4, maxFreq);
    }
    else
    {
        // LPF: extend along the bottom past the cutoff so the line reads as
        // "rolled off to silence."
        filterShape.lineTo(rightX, bottomY);
    }

    // --- Area path: same curve, closed at the bottom for a faint fill below.
    filterArea = filterShape;
    filterArea.lineTo(rightX, bottomY);
    filterArea.lineTo(x1,     bottomY);
    filterArea.closeSubPath();
}
