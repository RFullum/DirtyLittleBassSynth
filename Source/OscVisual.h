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
    
    void setOscShapeLine(AudioBuffer<float>& buffer);

private:
    float segmentThickness;
    
    Path oscShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscVisual)
};
