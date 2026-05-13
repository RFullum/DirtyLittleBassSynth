/*
  ==============================================================================

    ScopeVisual.cpp

  ==============================================================================
*/

#include "ScopeVisual.h"

//============================================================

ScopeVisual::ScopeVisual(const ScopeBuffer &sourceIn)
: source   (sourceIn)
, lineColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
, bgColor  (juce::Colour((juce::uint8)7,   (juce::uint8)10, (juce::uint8)59))
, fadeColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
{
    setOpaque(false);
    
    static_assert(captureSamples >= displaySamples * 2, "captureSamples must leave at least displaySamples of trigger headroom");
    static_assert((ScopeBuffer::bufferSize & (ScopeBuffer::bufferSize - 1)) == 0, "ScopeBuffer must be power of two");
}

void ScopeVisual::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    g.setGradientFill     (juce::ColourGradient::vertical(bgColor, fadeColor, visualBox));
    g.fillRoundedRectangle(visualBox, cornerRound);

    // Faint zero line.
    const float cy = visualBox.getCentreY();
    g.setColour(lineColor.withAlpha(0.15f));
    g.drawLine(visualBox.getX(), cy, visualBox.getRight(), cy, 0.5f);

    // Trace.
    g.setColour(lineColor);
    g.strokePath(scopePath
                 , juce::PathStrokeType(1.4f
                                        , juce::PathStrokeType::curved
                                        , juce::PathStrokeType::rounded));
}

void ScopeVisual::resized()
{
    constexpr int reducer = 2;
    auto bounds = getLocalBounds().reduced(reducer);

    visualBox.setBounds((float) bounds.getX(),     (float) bounds.getY(),
                        (float) bounds.getWidth(), (float) bounds.getHeight());
}

void ScopeVisual::Update()
{
    RebuildPath();
    repaint();
}

void ScopeVisual::SetColors(juce::Colour line, juce::Colour background, juce::Colour fade)
{
    lineColor = line;
    bgColor   = background;
    fadeColor = fade;
}

void ScopeVisual::RebuildPath()
{
    source.CopyLatest(scopeData, captureSamples);

    // Find a positive-going zero crossing somewhere in the first
    // (captureSamples - displaySamples) samples so we always have a full
    // displaySamples window after the trigger.
    const int searchLen = captureSamples - displaySamples;
    int       trig      = -1;

    for (int i = 1; i < searchLen; ++i)
    {
        if (scopeData[i - 1] <= 0.0f && scopeData[i] > 0.0f)
        {
            trig = i;
            break;
        }
    }

    const int start = (trig >= 0) ? trig : (captureSamples - displaySamples);

    scopePath.clear();

    if (visualBox.getWidth() <= 0.0f || visualBox.getHeight() <= 0.0f)
        return;

    const float padY      = 3.0f;
    const float left      = visualBox.getX();
    const float right     = visualBox.getRight();
    const float midY      = visualBox.getCentreY();
    const float halfH     = (visualBox.getHeight() * 0.5f) - padY;

    auto sampleY = [&](float s)
    {
        const float clamped = juce::jlimit(-1.0f, 1.0f, s);
        return midY - clamped * halfH;
    };

    auto sampleX = [&](int i)
    {
        return juce::jmap((float) i, 0.0f, (float) (displaySamples - 1), left, right);
    };

    scopePath.startNewSubPath(sampleX(0), sampleY(scopeData[start]));

    for (int i = 1; i < displaySamples; ++i)
        scopePath.lineTo(sampleX(i), sampleY(scopeData[start + i]));
}
