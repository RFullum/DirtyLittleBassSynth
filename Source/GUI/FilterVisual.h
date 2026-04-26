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
/*
*/
class FilterVisual  : public juce::Component
{
public:
    FilterVisual();
    ~FilterVisual() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    /**
     Draws the shape of the selected filter. Use filterType.getSelectedID()
     to find the filtType: 1 = -12dB LPF, 4 = Notch. 
     */
    void drawFilterShape(int filtType, float cutoff, float res);
    
    void setColors(juce::Colour& line1, juce::Colour& line2, juce::Colour& background, juce::Colour& fade);

private:
    void drawFilterShape(int type);
    
    juce::Rectangle<float> visualBox;
    
    juce::Path filterShape;
    
    float cutoffFreq;
    float resonance;
    float segmentThickness;
    float order;
    
    juce::Colour lineColor1;
    juce::Colour lineColor2;
    juce::Colour bgColor;
    juce::Colour bgFade;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterVisual)
};
