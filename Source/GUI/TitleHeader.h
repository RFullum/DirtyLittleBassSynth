/*
  ==============================================================================

    TitleHeader.h
    Created: 12 Jan 2021 11:10:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TitleHeader  : public juce::Component
{
public:
    TitleHeader();
    ~TitleHeader() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    /// Set the colors for the title header
    void setColors(juce::Colour& light, juce::Colour& dark, juce::Colour& text);

protected:
    juce::Colour lightColor;
    juce::Colour darkColor;
    juce::Colour textColor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TitleHeader)
};


//==============================================================================
//==============================================================================
//==============================================================================


class TitleFooter : public TitleHeader
{
public:
    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    juce::Rectangle<int> versionArea;
    juce::Rectangle<int> urlArea;
};


