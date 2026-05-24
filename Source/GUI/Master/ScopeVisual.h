/*
  ==============================================================================

    ScopeVisual.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ScopeBuffer.h"

//==============================================================================

// Oscilloscope: pulls from ScopeBuffer each tick, triggers on a positive-going
// zero crossing for waveform stability, paints the trace as a juce::Path.
class ScopeVisual
    : public juce::Component
{
public:
    ScopeVisual(const ScopeBuffer &source);
    ~ScopeVisual() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update();
    void SetColors(juce::Colour line, juce::Colour background, juce::Colour fade);

private:
    void RebuildPath();

    static constexpr int captureSamples = 2048;
    static constexpr int displaySamples = 1024;

    const ScopeBuffer &source;

    float scopeData[captureSamples] = {};

    juce::Colour lineColor;
    juce::Colour bgColor;
    juce::Colour fadeColor;

    juce::Rectangle<float> visualBox;
    juce::Path             scopePath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeVisual)
};
