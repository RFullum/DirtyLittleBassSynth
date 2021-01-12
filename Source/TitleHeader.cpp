/*
  ==============================================================================

    TitleHeader.cpp
    Created: 12 Jan 2021 11:10:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TitleHeader.h"

//==============================================================================
TitleHeader::TitleHeader()
{
}

TitleHeader::~TitleHeader()
{
}

void TitleHeader::paint (juce::Graphics& g)
{
    float cornerRound = 2.0f;
    
    Colour blazeOrange    =        ( Colour( (uint8)255, (uint8)46,  (uint8)0 ) );
    Colour sectionColorBlueDark  = ( Colour( (uint8)7,   (uint8)10, (uint8)59 ) );
    Colour sectionColourLightBlue = ( Colour( (uint8)143, (uint8)173, (uint8)186 ) );
    
    Rectangle<float> headerAreaInner = Rectangle<float>(getLocalBounds().getX(), getLocalBounds().getY(),
                                                        getLocalBounds().getWidth(), getLocalBounds().getHeight() );
    
    
    g.setGradientFill( ColourGradient::horizontal( sectionColourLightBlue, headerAreaInner.getX(),
                                                  sectionColorBlueDark, headerAreaInner.getWidth() * 0.75f) );
    g.fillRoundedRectangle( headerAreaInner, cornerRound );
    
    g.setColour ( blazeOrange );
    g.setFont   ( Font("technoid", 60.0f, 0) );
    g.drawText  ("FULLUMMUSIC", headerAreaInner.removeFromBottom( 45.0f ), Justification::right);
    
    headerAreaInner.removeFromRight( 3.0f);
    
    g.setColour ( sectionColourLightBlue );
    g.setFont   ( Font("technoid", 30.0f, 0) );
    g.drawText  ("DIRTY LITTLE BASS SYTNH", headerAreaInner.removeFromTop(25.0f), Justification::right);
    
}

void TitleHeader::resized()
{
}

