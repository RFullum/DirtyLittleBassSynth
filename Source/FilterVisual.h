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
    ~FilterVisual() override;

    void paint (Graphics&) override;
    void resized() override;
    
    /**
     Draws the shape of the selected filter. Use filterType.getSelectedID()
     to find the filtType: 1 = -12dB LPF, 4 = Notch. 
     */
    void drawFilterShape(int filtType, float cutoff, float res);
    
    void setColors(Colour& line1, Colour& line2, Colour& background);

private:
    void drawFilterShape(int type);
    
    Rectangle<float> visualBox;
    
    Path filterShape;
    
    float cutoffFreq;
    float resonance;
    float segmentThickness;
    float order;
    
    Colour lineColor1;
    Colour lineColor2;
    Colour bgColor;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterVisual)
};
