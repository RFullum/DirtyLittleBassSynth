/*
  ==============================================================================

    AdsrVisual.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//============================================================

/// Renders an attack/decay/sustain/release envelope curve from APVTS parameters.
/// Lives on the UI thread; the editor's timer drives Update() which re-reads the
/// four atomic parameter pointers and rebuilds the path.
class AdsrVisual
    : public juce::Component
{
public:
    AdsrVisual();
    ~AdsrVisual() override = default;

    /// One-time setup: which four parameters to follow. All ranges are taken from
    /// the parameter; the visual uses A/D/R seconds (0..maxTime) and S as 0..1.
    void Init(std::atomic<float> *attackParamIn
              , std::atomic<float> *decayParamIn
              , std::atomic<float> *sustainParamIn
              , std::atomic<float> *releaseParamIn);

    /// Sets the wave-line color, background color, and bottom fade color.
    void SetColors(juce::Colour line, juce::Colour background, juce::Colour fade);

    /// Reads the four parameters, rebuilds the envelope path, and triggers a repaint.
    /// Call from the editor's timer.
    void Update();

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void RebuildPath();

    std::atomic<float> *attackParam  = nullptr;
    std::atomic<float> *decayParam   = nullptr;
    std::atomic<float> *sustainParam = nullptr;
    std::atomic<float> *releaseParam = nullptr;

    juce::Colour lineColor;
    juce::Colour bgColor;
    juce::Colour fadeColor;

    juce::Rectangle<float> visualBox;

    juce::Path envShape;
    juce::Path envArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdsrVisual)
};
