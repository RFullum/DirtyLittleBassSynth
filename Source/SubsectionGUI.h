/*
  ==============================================================================

    SubsectionGUI.h
    Created: 30 Dec 2020 5:06:29pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SubsectionGUI  : public juce::Component
{
public:
    SubsectionGUI();
    ~SubsectionGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Sets the color that fills tha background of the section
    void setFillColor(Colour& c);   
private:
    // Colors
    Colour fillColor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubsectionGUI)
};
