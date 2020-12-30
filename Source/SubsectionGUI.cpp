/*
  ==============================================================================

    SubsectionGUI.cpp
    Created: 30 Dec 2020 5:06:29pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SubsectionGUI.h"

//==============================================================================
SubsectionGUI::SubsectionGUI() : fillColor( Colour( (uint8)0, (uint8)0, (uint8)0 ) )
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

SubsectionGUI::~SubsectionGUI()
{
}

void SubsectionGUI::paint (juce::Graphics& g)
{
    g.fillAll ( fillColor );   // clear the background
}

void SubsectionGUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


void SubsectionGUI::setFillColor(Colour& c)
{
    fillColor = c;
}
