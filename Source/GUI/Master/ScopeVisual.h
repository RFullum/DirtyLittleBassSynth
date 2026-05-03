/*
  ==============================================================================

    ScopeVisual.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ScopeBuffer.h"

//============================================================

/// Oscilloscope display. Pulls samples from a ScopeBuffer each timer tick, finds
/// a positive-going zero crossing for waveform stability, and renders the
/// resulting trace as a juce::Path. Background gradient + faint zero-line match
/// the OscVisual / AdsrVisual style.
class ScopeVisual
    : public juce::Component
{
public:
    ScopeVisual(const ScopeBuffer &source);
    ~ScopeVisual() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    /// Reads the latest samples from the buffer, applies the trigger, and
    /// repaints. Call from the editor's timer.
    void Update();

    /// Sets the trace, background, and bottom-fade colors.
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
