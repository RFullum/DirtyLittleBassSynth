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
FilterVisual::FilterVisual() : cutoffFreq(100.0f), resonance(0.1f), segmentThickness(1.0f),
                               lineColor1 ( juce::Colour( (juce::uint8)255, (juce::uint8)94, (juce::uint8)0  ) ),
                               lineColor2 ( juce::Colour( (juce::uint8)100, (juce::uint8)36, (juce::uint8)0  ) ),
                               bgColor    ( juce::Colour( (juce::uint8)7,   (juce::uint8)10, (juce::uint8)59 ) ),
                               bgFade     ( juce::Colour( (juce::uint8)100, (juce::uint8)36, (juce::uint8)0  ) )
{}


void FilterVisual::setColors(juce::Colour line1, juce::Colour line2, juce::Colour background, juce::Colour fade)
{
    lineColor1 = line1;
    lineColor2 = line2;
    bgColor    = background;
    bgFade     = fade;
}


void FilterVisual::paint (juce::Graphics& g)
{
    float cornerRound = 2.0f;
    
    g.setGradientFill      ( juce::ColourGradient::vertical( bgColor, bgFade, visualBox ) );
    g.fillRoundedRectangle ( visualBox, cornerRound );
    
    g.setColour            ( lineColor1 );
    g.setFillType          ( juce::ColourGradient::vertical( lineColor1, lineColor2, visualBox ) );
    g.fillPath             ( filterShape );
}

void FilterVisual::resized()
{
    int reducer    = 2;
    auto totalArea = getLocalBounds();
    
    juce::Rectangle<int> reducedArea = totalArea.reduced( reducer );
    
    visualBox.setBounds( reducedArea.getX(), reducedArea.getY(),
                         reducedArea.getWidth(), reducedArea.getHeight() );
}


void FilterVisual::drawFilterShape(int filtType, float cutoff, float res)
{
    cutoffFreq = cutoff;
    resonance  = res;
    
    filterShape.clear();
    
    switch (filtType)
    {
        case 1:
            drawFilterShape(filtType);
            break;
        case 2:
            drawFilterShape(filtType);
            break;
        case 3:
            drawFilterShape(filtType);
            break;
        case 4:
            drawFilterShape(filtType);
            break;
        default:
            drawFilterShape(filtType);
            break;
    }
    
    
    repaint();
}

void FilterVisual::drawFilterShape(int type)
{
    float halfHeight = getHeight() * 0.5f;
    float resMap     = juce::jmap( resonance, 1.0f, 2.0f, 0.0f, halfHeight * 2.5f ); // for y2, to get res height for ctrlPt2
    
    float rollOffControl;   // moves ctrlPt1 & ctrlPt2 to adjust lpf order rolloff
    
    switch (type)
    {
        // Controls how steep the rolloff curve is
        case 1:
            rollOffControl = 66.0f;
            break;
        case 2:
            rollOffControl = 33.0f;
            break;
        case 3:
            rollOffControl = 11.0f;
            break;
        case 4:
            rollOffControl = 33.0f;
            break;
        default:
            rollOffControl = 66.0f;
            break;
    }
    
    float reducer = 2.0f;
    
    float x1 = 0.0f + reducer;                                  // Top left of path: Left
    float y1 = getHeight() * 0.5f;                              // Top left of path: halfHeight
    float x2 = juce::jmap( powf( cutoffFreq * 0.01f, 0.25f),
                    0.32f, 1.0f,
                    15.0f, (float)getWidth() - reducer );       // cutoff freq
    float y2 = ( halfHeight ) - resMap;                         // halfHeight, minus (up) to resonance
    
    juce::Point<float> origin    ( x1, y1 );
    juce::Point<float> maxFilter ( x2, getHeight() - reducer );
    juce::Point<float> ctrlPt1   ( x2 - rollOffControl, halfHeight );
    juce::Point<float> ctrlPt2   ( x2 - rollOffControl, y2 );
    
    if (ctrlPt1.getX() < reducer)
        ctrlPt1.setX(reducer);
    
    if (ctrlPt2.getX() < reducer)
        ctrlPt2.setX(reducer);
    
    filterShape.startNewSubPath ( x1, y1 );
    filterShape.cubicTo         ( ctrlPt1, ctrlPt2, maxFilter );
    
    
    if (type == 4)
    {
        float x3 = getWidth() - reducer;
        float y3 = halfHeight;
        float x4 = x2 + rollOffControl;//(x2 - (x2 * rollOffControl) );  // *** THIS IS BEHAVING FUNNY? ***
        float y4 = y2;
        
        juce::Point<float> maxFreq( x3, y3 );
        juce::Point<float> ctrlPt3( x4, y4 );
        juce::Point<float> ctrlPt4( x4, halfHeight );
        
        if (ctrlPt3.getX() > getWidth() - reducer)
            ctrlPt3.setX(getWidth() - reducer);
        
        if (ctrlPt4.getX() > getWidth() - reducer)
            ctrlPt4.setX(getWidth() - reducer);
        
        filterShape.cubicTo ( ctrlPt3, ctrlPt4, maxFreq ); 
        filterShape.lineTo  ( getWidth() - reducer, getHeight() - reducer );
    }
    
    filterShape.lineTo( x1, (float)getHeight() - reducer );
}

