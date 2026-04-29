/*
  ==============================================================================

    OutMeter.h
    Created: 10 Jan 2021 2:32:52pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

/// Stereo output meter: per-channel level bars with a clip strip at the top of each.
class OutMeter
    : public juce::Component
{
public:
    OutMeter();
    ~OutMeter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    /// Pushes new per-channel magnitudes into the meter and recomputes display heights.
    void outMeterLevel(float leftLevel, float rightLevel, float sampleRate);

    /// Sets the colors of the level fill and the clip-strip lit state.
    void setColors(juce::Colour levelColor, juce::Colour clipColor);

private:
    void updateHeight(float &heightMult, float magnitude);

    juce::Rectangle<int> leftClipRect;
    juce::Rectangle<int> rightClipRect;
    juce::Rectangle<int> leftMeterBack;
    juce::Rectangle<int> rightMeterBack;
    juce::Rectangle<int> leftMeterLevel;
    juce::Rectangle<int> rightMeterLevel;

    bool  leftClipping     = false;
    bool  rightClipping    = false;
    float leftHeightMult   = 0.0f;
    float rightHeightMult  = 0.0f;

    float SR              = 44100.0f;
    float decayRateRise   = 0.0005f;
    float decayRateFall   = 0.001f;
    float decayFactorRise = decayRateRise * SR;
    float decayFactorFall = decayRateFall * SR;

    juce::Colour clipLitColor;
    juce::Colour clipDimColor;
    juce::Colour levelColor;
    juce::Colour levelBackColor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutMeter)
};
