/*
  ==============================================================================

    OscVisual.h
    Created: 29 Dec 2020 12:01:58pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OscVisual  : public juce::Component
{
public:
    OscVisual();
    ~OscVisual() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    /// Concatenates juce::Path line segments acress visualBox using sample values in buffer
    void setOscShapeLine(juce::AudioBuffer<float>& buffer);
    
    /// Sets the colors of the wave line and background area
    void setColors(juce::Colour& line, juce::Colour& background, juce::Colour& fade);

private:
    float segmentThickness;
    
    juce::Colour lineColor;
    juce::Colour bgColor;
    juce::Colour fadeColor;
    
    juce::Rectangle<float> visualBox;
    juce::Rectangle<float> visualBoxReduced;
    
    juce::Path oscShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscVisual)
};

