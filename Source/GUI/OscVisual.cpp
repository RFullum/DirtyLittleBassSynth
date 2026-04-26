/*
  ==============================================================================

    OscVisual.cpp
    Created: 29 Dec 2020 12:01:58pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OscVisual.h"

//==============================================================================

OscVisual::OscVisual()
: segmentThickness(1.5f)
, lineColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
, bgColor  (juce::Colour((juce::uint8)7,   (juce::uint8)10, (juce::uint8)59))
, fadeColor(juce::Colour((juce::uint8)255, (juce::uint8)94, (juce::uint8)0))
{
    // Populate the four shape wavetables once. Sample rate is irrelevant for the
    // displayed shape; any value just produces one cycle across waveTableSize samples.
    wtSine  .SetSampleRate(44100.0f);
    wtSpike .SetSampleRate(44100.0f);
    wtSquare.SetSampleRate(44100.0f);
    wtSaw   .SetSampleRate(44100.0f);

    wtSine  .PopulateWavetable();
    wtSpike .PopulateWavetable();
    wtSquare.PopulateWavetable();
    wtSaw   .PopulateWavetable();
}

void OscVisual::Init(std::atomic<float> *morphParamIn, bool useSquareIn)
{
    morphParam = morphParamIn;
    useSquare  = useSquareIn;
}

void OscVisual::Update()
{
    if (morphParam == nullptr)
        return;

    RebuildPath();
    repaint();
}

void OscVisual::SetColors(juce::Colour &line, juce::Colour &background, juce::Colour &fade)
{
    lineColor = line;
    bgColor   = background;
    fadeColor = fade;
}

void OscVisual::paint(juce::Graphics &g)
{
    float cornerRound = 2.0f;

    g.setGradientFill     (juce::ColourGradient::vertical(bgColor, fadeColor, visualBox));
    g.fillRoundedRectangle(visualBox, cornerRound);

    g.setColour(lineColor);
    g.fillPath (oscShape);
}

void OscVisual::resized()
{
    int  reducer     = 2;
    auto totalArea   = getLocalBounds();
    auto reducedArea = totalArea.reduced(reducer);

    visualBox.setBounds(reducedArea.getX(), reducedArea.getY(), reducedArea.getWidth(), reducedArea.getHeight());
}

void OscVisual::RebuildPath()
{
    // Read the morph parameter and convert to per-shape gain levels.
    float sinLevel    = morphControl.sinMorphGain  (morphParam);
    float centerLevel = morphControl.spikeMorphGain(morphParam);
    float sawLevel    = morphControl.sawMorphGain  (morphParam);

    oscShape.clear();

    float widthOffset  = 5.0f;
    float widthReduce  = getWidth() - widthOffset;
    float heightReduce = getHeight() * 0.77f;
    float halfHeight   = getHeight() * 0.5f;

    auto sampleAt = [&](int i)
    {
        float center = useSquare
                          ? centerLevel * wtSquare.GetWavetableSampleValue(i)
                          : centerLevel * wtSpike .GetWavetableSampleValue(i);

        return sinLevel * wtSine.GetWavetableSampleValue(i)
             + center
             + sawLevel * wtSaw .GetWavetableSampleValue(i);
    };

    for (int i = 0; i < waveTableSize - 1; ++i)
    {
        float x1 = juce::jmap((float) i,         0.0f, (float) waveTableSize, widthReduce, widthOffset);
        float y1 = juce::jmap(sampleAt(i),       halfHeight, heightReduce);
        float x2 = juce::jmap((float) (i + 1),   0.0f, (float) waveTableSize, widthReduce, widthOffset);
        float y2 = juce::jmap(sampleAt(i + 1),   halfHeight, heightReduce);

        oscShape.addLineSegment(juce::Line<float>(x1, y1, x2, y2), segmentThickness);
    }
}
