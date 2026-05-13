/*
  ==============================================================================

    OscVisual.cpp

  ==============================================================================
*/

#include "OscVisual.h"

//============================================================

OscVisual::OscVisual()
: lineColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
, bgColor  (juce::Colour((juce::uint8)7,   (juce::uint8)10, (juce::uint8)59))
, fadeColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
{
    setOpaque(false);
    
    wtSine  .SetSampleRate(44100.0f);
    wtSpike .SetSampleRate(44100.0f);
    wtSquare.SetSampleRate(44100.0f);
    wtSaw   .SetSampleRate(44100.0f);

    wtSine  .PopulateWavetable();
    wtSpike .PopulateWavetable();
    wtSquare.PopulateWavetable();
    wtSaw   .PopulateWavetable();
}

void OscVisual::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    // Background fill
    g.setGradientFill     (juce::ColourGradient::vertical(bgColor, fadeColor, visualBox));
    g.fillRoundedRectangle(visualBox, cornerRound);

    // Center reference line
    const float cy = (float)getHeight() * 0.5f;
    g.setColour(lineColor.withAlpha(0.15f));
    g.drawLine(0.0f, cy, (float)getWidth(), cy, 0.5f);

    // Faint area fill below the wave
    g.setColour(lineColor.withAlpha(0.08f));
    g.fillPath(oscArea);

    // Wave line
    g.setColour(lineColor);
    g.strokePath(oscShape,
                 juce::PathStrokeType(1.4f,
                                      juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));
}

void OscVisual::resized()
{
    int  reducer     = 2;
    auto bounds      = getLocalBounds();
    auto reducedArea = bounds.reduced(reducer);

    visualBox.setBounds(reducedArea.getX(), reducedArea.getY(), reducedArea.getWidth(), reducedArea.getHeight());
}

void OscVisual::Init(std::atomic<float> *morphParamIn, bool useSquareIn, std::atomic<float> *gainParamIn)
{
    morphParam = morphParamIn;
    gainParam  = gainParamIn;
    useSquare  = useSquareIn;
}

void OscVisual::Update()
{
    if (morphParam == nullptr)
        return;

    RebuildPath();
    repaint();
}

void OscVisual::SetColors(juce::Colour line, juce::Colour background, juce::Colour fade)
{
    lineColor = line;
    bgColor   = background;
    fadeColor = fade;
}

void OscVisual::RebuildPath()
{
    // Read the morph parameter and convert to per-shape gain levels.
    const float sinLevel    = morphControl.SinMorphGain  (morphParam);
    const float centerLevel = morphControl.SpikeMorphGain(morphParam);
    const float sawLevel    = morphControl.SawMorphGain  (morphParam);

    oscShape.clear();
    oscArea .clear();

    const float widthOffset  = 5.0f;
    const float widthReduce  = (float)getWidth() - widthOffset;
    const float heightReduce = (float)getHeight() * 0.77f;
    const float halfHeight   = (float)getHeight() * 0.5f;
    const float bottomY      = (float)getHeight();

    const float gainScale = (gainParam != nullptr)
                                ? juce::jlimit(0.0f, 1.0f, gainParam->load())
                                : 1.0f;

    auto sampleAt = [&](int i)
    {
        const float center = useSquare
                                ? centerLevel * wtSquare.GetWavetableSampleValue(i)
                                : centerLevel * wtSpike .GetWavetableSampleValue(i);

        const float sample = sinLevel * wtSine.GetWavetableSampleValue(i)
                           + center
                           + sawLevel * wtSaw .GetWavetableSampleValue(i);

        return sample * gainScale;
    };

    auto sampleX = [&](int i)
    {
        return juce::jmap((float)i, 0.0f, (float)waveTableSize, widthReduce, widthOffset);
    };

    auto sampleY = [&](int i)
    {
        return juce::jmap(sampleAt(i), halfHeight, heightReduce);
    };

    // Connected line path through every wavetable sample.
    oscShape.startNewSubPath(sampleX(0), sampleY(0));
    for (int i = 1; i < waveTableSize; ++i)
        oscShape.lineTo(sampleX(i), sampleY(i));

    // Same line, closed at the bottom for the area fill.
    oscArea.startNewSubPath(sampleX(0), sampleY(0));
    for (int i = 1; i < waveTableSize; ++i)
        oscArea.lineTo(sampleX(i), sampleY(i));
    
    oscArea.lineTo(sampleX(waveTableSize - 1), bottomY);
    oscArea.lineTo(sampleX(0), bottomY);
    oscArea.closeSubPath();
}
