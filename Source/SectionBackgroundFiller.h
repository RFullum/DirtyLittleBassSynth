/*
  ==============================================================================

    SectionBackgroundFiller.h
    Created: 30 Dec 2020 11:12:31am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SectionBackgroundFiller  : public juce::Component
{
public:
    SectionBackgroundFiller();
    ~SectionBackgroundFiller() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setFillColor(Colour& c);

private:
    Colour fillColor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SectionBackgroundFiller)
};
