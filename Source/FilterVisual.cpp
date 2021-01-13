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
FilterVisual::FilterVisual() : sampleRate(44100.0f), cutoffFreq(100.0f),
                               resonance(0.1f), segmentThickness(1.0f)
{
}

FilterVisual::~FilterVisual()
{
}

void FilterVisual::paint (juce::Graphics& g)
{
    float cornerRound = 2.0f;
    
    Colour lineColor   = Colour( (uint8)255, (uint8)94, (uint8)0  );
    Colour lineColor2  = Colour( (uint8)100, (uint8)36, (uint8)0  );
    Colour sectionBlue = Colour( (uint8)7,   (uint8)10, (uint8)59 );
    
    g.setGradientFill      ( ColourGradient::vertical( Colours::black, sectionBlue, visualBox ) );
    g.fillRoundedRectangle ( visualBox, cornerRound );
    g.setColour            ( lineColor );
    g.setFillType          ( ColourGradient::vertical( lineColor, lineColor2, visualBox ) );
    g.fillPath             ( filterShape );
}

void FilterVisual::resized()
{
    int reducer    = 2;
    auto totalArea = getLocalBounds();
    
    Rectangle<int> reducedArea = totalArea.reduced( reducer );
    
    visualBox.setBounds( reducedArea.getX(), reducedArea.getY(),
                         reducedArea.getWidth(), reducedArea.getHeight() );
}


void FilterVisual::drawFilterShape(int filtType, float SR, float cutoff, float res)
{
    sampleRate = SR;
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
    float resMap     = jmap( resonance, 1.0f, 2.0f, 0.0f, halfHeight * 2.5f ); // for y2, to get res height for ctrlPt2
    
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
    float x2 = jmap( powf( cutoffFreq * 0.01f, 0.25f),
                    0.32f, 1.0f,
                    15.0f, (float)getWidth() - reducer );       // cutoff freq
    float y2 = ( halfHeight ) - resMap;                         // halfHeight, minus (up) to resonance
    
    Point<float> origin    ( x1, y1 );
    Point<float> maxFilter ( x2, getHeight() - reducer );
    Point<float> ctrlPt1   ( x2 - rollOffControl, halfHeight );
    Point<float> ctrlPt2   ( x2 - rollOffControl, y2 );
    
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
        
        Point<float> maxFreq( x3, y3 );
        Point<float> ctrlPt3( x4, y4 );
        Point<float> ctrlPt4( x4, halfHeight );
        
        if (ctrlPt3.getX() > getWidth() - reducer)
            ctrlPt3.setX(getWidth() - reducer);
        
        if (ctrlPt4.getX() > getWidth() - reducer)
            ctrlPt4.setX(getWidth() - reducer);
        
        filterShape.cubicTo ( ctrlPt3, ctrlPt4, maxFreq ); 
        filterShape.lineTo  ( getWidth() - reducer, getHeight() - reducer );
    }
    
    filterShape.lineTo( x1, (float)getHeight() - reducer );
}

