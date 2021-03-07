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
class OscVisual  : public Component
{
public:
    OscVisual();
    ~OscVisual() override;

    void paint (Graphics&) override;
    void resized() override;
    
    /// Concatenates Path line segments acress visualBox using sample values in buffer
    void setOscShapeLine(AudioBuffer<float>& buffer);
    
    /// Sets the colors of the wave line and background area
    void setColors(Colour& line, Colour& background);

private:
    float segmentThickness;
    
    Colour lineColor;
    Colour bgColor;
    
    Rectangle<float> visualBox;
    
    Path oscShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscVisual)
};

