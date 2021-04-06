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
TitleHeader::TitleHeader() : lightColor( Colour( (uint8)143, (uint8)173, (uint8)186 ) ),
                             darkColor ( Colour( (uint8)7,   (uint8)10,  (uint8)59  ) ),
                             textColor ( Colour( (uint8)255, (uint8)46,  (uint8)0   ) )
{}

TitleHeader::~TitleHeader() {}

/// Sets the colors of the TitleHeader
void TitleHeader::setColors(Colour& light, Colour& dark, Colour& text)
{
    lightColor = light;
    darkColor  = dark;
    textColor  = text;
}

void TitleHeader::paint (juce::Graphics& g)
{
    float cornerRound = 2.0f;
    
    Rectangle<float> headerAreaInner = Rectangle<float>( getLocalBounds().getX(), getLocalBounds().getY(),
                                                         getLocalBounds().getWidth(), getLocalBounds().getHeight() );
    
    g.setGradientFill      ( ColourGradient::horizontal( lightColor, headerAreaInner.getX(),
                                                         darkColor, headerAreaInner.getWidth() * 0.38f ) );
    g.fillRoundedRectangle ( headerAreaInner, cornerRound );
    
    g.setColour ( textColor);
    g.setFont   ( Font("technoid", 60.0f, 0) );
    g.drawText  ( "FULLUMMUSIC", headerAreaInner.removeFromBottom( 45.0f ), Justification::right );
    
    headerAreaInner.removeFromRight( 3.0f );
    
    g.setColour ( lightColor );
    g.setFont   ( Font("technoid", 30.0f, 0) );
    g.drawText  ( "DIRTY LITTLE BASS SYTNH", headerAreaInner.removeFromTop(25.0f), Justification::right );
    
}

void TitleHeader::resized() {}



//==============================================================================
//==============================================================================
//==============================================================================



void TitleFooter::paint(juce::Graphics& g)
{
    g.fillAll   ( darkColor );
    g.setColour ( textColor );
    
    g.setFont  ( Font("Helvetica", 14, 0) );
    g.drawText ( "Version 1.1.2", versionArea.reduced( 2, 6 ), Justification::left );
    g.drawText ( "www.FullumMusic.com", urlArea.reduced( 2, 6 ), Justification::right );
}


void TitleFooter::resized()
{
    auto totalArea = getLocalBounds();
    
    versionArea = totalArea.removeFromLeft( totalArea.getWidth() * 0.5f );
    urlArea     = totalArea;
}
