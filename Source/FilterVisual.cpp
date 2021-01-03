/*
  ==============================================================================

    FilterVisual.cpp
    Created: 1 Jan 2021 6:08:24pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterVisual.h"

//==============================================================================
FilterVisual::FilterVisual() : segmentThickness(1.0f), sampleRate(44100.0f),
                               cutoffFreq(18000.0f), resonance(0.1f)
{
}

FilterVisual::~FilterVisual()
{
}

void FilterVisual::paint (juce::Graphics& g)
{
    float cornerRound = 10.0f;
    
    g.setColour            ( Colours::grey );
    g.fillRoundedRectangle ( visualBox, cornerRound );
    g.setColour            ( Colours::black) ;
    g.fillPath             ( filterShape );
}

void FilterVisual::resized()
{
    int reducer    = 2;
    auto totalArea = getLocalBounds();
    
    Rectangle<int> reducedArea = totalArea.reduced( reducer );
    
    visualBox.setBounds( reducedArea.getX(), reducedArea.getY(), reducedArea.getWidth(), reducedArea.getHeight() );
}


void FilterVisual::drawFilterShape(int filtType, float SR, float cutoff, float res)
{
    sampleRate = SR;
    cutoffFreq = cutoff;
    resonance = res;    
    
}

