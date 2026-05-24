/*
  ==============================================================================

    AdsrVisual.cpp

  ==============================================================================
*/

#include "AdsrVisual.h"

//==============================================================================

AdsrVisual::AdsrVisual()
: lineColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
, bgColor  (juce::Colour((juce::uint8)7,   (juce::uint8)10, (juce::uint8)59))
, fadeColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
{
    setOpaque(false);
}

void AdsrVisual::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    g.setGradientFill     (juce::ColourGradient::vertical(bgColor, fadeColor, visualBox));
    g.fillRoundedRectangle(visualBox, cornerRound);

    const float baseY = visualBox.getBottom() - 1.0f;
    g.setColour(lineColor.withAlpha(0.15f));
    g.drawLine(visualBox.getX(), baseY, visualBox.getRight(), baseY, 0.5f);

    g.setColour(lineColor.withAlpha(0.08f));
    g.fillPath(envArea);

    g.setColour(lineColor);
    g.strokePath(envShape, juce::PathStrokeType(1.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void AdsrVisual::resized()
{
    static constexpr int reducer = 2;

    auto bounds      = getLocalBounds();
    auto reducedArea = bounds.reduced(reducer);

    visualBox.setBounds((float)reducedArea.getX()
                        , (float)reducedArea.getY()
                        , (float)reducedArea.getWidth()
                        , (float)reducedArea.getHeight());
}

void AdsrVisual::Init(std::atomic<float>   *attackParamIn
                      , std::atomic<float> *decayParamIn
                      , std::atomic<float> *sustainParamIn
                      , std::atomic<float> *releaseParamIn)
{
    attackParam  = attackParamIn;
    decayParam   = decayParamIn;
    sustainParam = sustainParamIn;
    releaseParam = releaseParamIn;
}

void AdsrVisual::SetColors(juce::Colour line, juce::Colour background, juce::Colour fade)
{
    lineColor = line;
    bgColor   = background;
    fadeColor = fade;
}

void AdsrVisual::Update()
{
    if (attackParam == nullptr)
        return;

    RebuildPath();
    repaint();
}

void AdsrVisual::RebuildPath()
{
    const float a = attackParam ->load();
    const float d = decayParam  ->load();
    const float s = juce::jlimit(0.0f, 1.0f, sustainParam->load());
    const float r = releaseParam->load();

    // Synthetic hold segment so the sustain plateau is always visible at any total length.
    const float hold      = juce::jmax(0.25f, (a + d + r) * 0.25f);
    const float totalTime = juce::jmax(0.001f, a + d + hold + r);

    const float padX     = 4.0f;
    const float padTop   = 3.0f;
    const float padBot   = 2.0f;
    const float plotL    = visualBox.getX()      + padX;
    const float plotR    = visualBox.getRight()  - padX;
    const float plotTop  = visualBox.getY()      + padTop;
    const float plotBot  = visualBox.getBottom() - padBot;
    const float plotW    = plotR - plotL;
    const float plotH    = plotBot - plotTop;

    auto timeToX  = [&](float t)     { return plotL + (t / totalTime) * plotW; };
    auto levelToY = [&](float level) { return plotBot - juce::jlimit(0.0f, 1.0f, level) * plotH; };

    const float x0 = timeToX(0.0f);
    const float x1 = timeToX(a);
    const float x2 = timeToX(a + d);
    const float x3 = timeToX(a + d + hold);
    const float x4 = timeToX(a + d + hold + r);

    const float y0    = levelToY(0.0f);
    const float yPeak = levelToY(1.0f);
    const float yS    = levelToY(s);

    envShape.clear();
    envArea .clear();

    envShape.startNewSubPath(x0, y0);
    envShape.lineTo         (x1, yPeak);
    envShape.lineTo         (x2, yS);
    envShape.lineTo         (x3, yS);
    envShape.lineTo         (x4, y0);

    envArea.startNewSubPath(x0, y0);
    envArea.lineTo         (x1, yPeak);
    envArea.lineTo         (x2, yS);
    envArea.lineTo         (x3, yS);
    envArea.lineTo         (x4, y0);
    envArea.closeSubPath();
}
