/*
  ==============================================================================

    SectionBackgroundFiller.cpp
    Created: 30 Dec 2020 11:12:31am
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SectionBackgroundFiller.h"

//==============================================================================
SectionBackgroundFiller::SectionBackgroundFiller() : fillColor( Colour( (uint8)0, (uint8)0, (uint8)0 ) )
{
}

SectionBackgroundFiller::~SectionBackgroundFiller()
{
}

void SectionBackgroundFiller::paint (juce::Graphics& g)
{
    g.fillAll (fillColor);
}

void SectionBackgroundFiller::resized()
{
}

/// Pass a Colour reference to fill the background of a section this color.
void SectionBackgroundFiller::setFillColor(Colour& c)
{
    fillColor = c;
}
