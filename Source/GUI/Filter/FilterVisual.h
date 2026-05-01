/*
  ==============================================================================

    FilterVisual.h
    Created: 1 Jan 2021 6:08:24pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class FilterVisual
    : public juce::Component
{
public:
    FilterVisual();
    ~FilterVisual() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    /// Rebuilds the displayed shape from the current filter type / cutoff / resonance.
    /// filtType: 1 = -12dB LPF, 2 = -24dB LPF, 3 = -48dB LPF, 4 = Notch.
    void drawFilterShape(int filtType, float cutoff, float res);

    void setColors(juce::Colour line, juce::Colour background, juce::Colour fade);

private:
    void buildPaths(int type);

    juce::Rectangle<float> visualBox;

    juce::Path filterShape;     // open line for stroke
    juce::Path filterArea;      // closed shape for the area fill below the line

    float cutoffFreq = 100.0f;
    float resonance  = 0.1f;

    juce::Colour lineColor;
    juce::Colour bgColor;
    juce::Colour fadeColor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterVisual)
};
