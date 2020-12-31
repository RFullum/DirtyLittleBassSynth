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
OscVisual::OscVisual() : segmentThickness(1.0f)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}

OscVisual::~OscVisual()
{
}

void OscVisual::paint (Graphics& g)
{
    float cornerRound = 10.0f;
    //g.fillAll (Colours::grey);   // clear the background
    g.setColour(Colours::grey);
    g.fillRoundedRectangle( visualBox, cornerRound );
    g.setColour(Colours::black);
    g.fillPath(oscShape);
}

void OscVisual::resized()
{
    int reducer    = 2;
    auto totalArea = getLocalBounds();
    
    Rectangle<int> reducedArea = totalArea.reduced( reducer );
    
    visualBox.setBounds( reducedArea.getX(), reducedArea.getY(), reducedArea.getWidth(), reducedArea.getHeight() );
}

/// Concatenates Path line segments acress visualBox using sample values in buffer
void OscVisual::setOscShapeLine(AudioBuffer<float>& buffer)
{
    float numSamples = (float) buffer.getNumSamples();
    
    oscShape.clear();
    
    for (int i=0; i<numSamples-1; i++)
    {
        float widthOffset  = 5.0f;
        float widthReduce  = getWidth() - widthOffset;
        float heightReduce = getHeight() * 0.77f;
        float halfHeight   = getHeight() * 0.5f;
        
        float x1 = jmap( (float)i, 0.0f, numSamples, widthReduce, widthOffset );
        float y1 = jmap( buffer.getSample(0, i), halfHeight, heightReduce );
        
        float x2 = jmap( (float)i + 1.0f, 0.0f, numSamples, widthReduce, widthOffset );
        float y2 = jmap( buffer.getSample(0, i+1), halfHeight, heightReduce );
        
        oscShape.addLineSegment( Line<float>(x1, y1, x2, y2), segmentThickness) ;
    }
    
    repaint();

}
