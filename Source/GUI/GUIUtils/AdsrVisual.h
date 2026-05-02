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

    void paint(juce::Graphics &) override;
    void resized() override;
    
    void Init(std::atomic<float>   *attackParamIn
              , std::atomic<float> *decayParamIn
              , std::atomic<float> *sustainParamIn
              , std::atomic<float> *releaseParamIn);
    void SetColors(juce::Colour line, juce::Colour background, juce::Colour fade);
    void Update();


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
