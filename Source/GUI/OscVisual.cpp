/*
  ==============================================================================

    OscVisual.cpp
    Created: 29 Dec 2020 12:01:58pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscVisual.h"

//==============================================================================
OscVisual::OscVisual() : segmentThickness(1.5f),
                         lineColor    ( juce::Colour( (juce::uint8)255, (juce::uint8)94, (juce::uint8)0  ) ),
                         bgColor      ( juce::Colour( (juce::uint8)7,   (juce::uint8)10, (juce::uint8)59 ) ),
                         fadeColor    ( juce::Colour( (juce::uint8)255, (juce::uint8)94, (juce::uint8)0  ) )
{}

OscVisual::~OscVisual() {}

/// Sets the colors of the wave line and background area
void OscVisual::setColors(juce::Colour& line, juce::Colour& background, juce::Colour& fade)
{
    lineColor = line;
    bgColor   = background;
    fadeColor = fade;
}

void OscVisual::paint (juce::Graphics& g)
{
    float cornerRound = 2.0f;
    
    g.setGradientFill      ( juce::ColourGradient::vertical( bgColor, fadeColor, visualBox ) );
    g.fillRoundedRectangle ( visualBox, cornerRound );
    
    g.setColour            ( lineColor );
    g.fillPath             ( oscShape  );
}

void OscVisual::resized()
{
    int reducer    = 2;
    auto totalArea = getLocalBounds();
    
    juce::Rectangle<int> reducedArea = totalArea.reduced( reducer );
    
    visualBox.setBounds( reducedArea.getX(), reducedArea.getY(), reducedArea.getWidth(), reducedArea.getHeight() );
}

/// Concatenates juce::Path line segments acress visualBox using sample values in buffer
void OscVisual::setOscShapeLine(juce::AudioBuffer<float>& buffer)
{
    float numSamples = (float) buffer.getNumSamples();
    
    oscShape.clear();
    
    for (int i=0; i<numSamples-1; i++)
    {
        float widthOffset  = 5.0f;
        float widthReduce  = getWidth() - widthOffset;
        float heightReduce = getHeight() * 0.77f;
        float halfHeight   = getHeight() * 0.5f;
        
        float x1 = juce::jmap( (float)i, 0.0f, numSamples, widthReduce, widthOffset );
        float y1 = juce::jmap( buffer.getSample(0, i), halfHeight, heightReduce );
        
        float x2 = juce::jmap( (float)i + 1.0f, 0.0f, numSamples, widthReduce, widthOffset );
        float y2 = juce::jmap( buffer.getSample(0, i+1), halfHeight, heightReduce );
        
        oscShape.addLineSegment( juce::Line<float>(x1, y1, x2, y2), segmentThickness) ;
    }
    
    repaint();

}
