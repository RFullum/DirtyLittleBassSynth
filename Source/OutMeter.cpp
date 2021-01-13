/*
  ==============================================================================

    OutMeter.cpp
    Created: 10 Jan 2021 2:32:52pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OutMeter.h"

//==============================================================================
OutMeter::OutMeter() : outLevel(0.0f), levelClipping(false), heightMult(0.0f),
                       SR(44100), decayRateRise(0.0005f), decayRateFall(0.001f),
                       decayFactorRise(decayRateRise * SR), decayFactorFall(decayRateFall * SR)
{
}

OutMeter::~OutMeter()
{
}

void OutMeter::paint (Graphics& g)
{
    Colour clipBackRed    = Colour( (uint8)97,  (uint8)9,   (uint8)5 );
    Colour clippingRed    = Colour( (uint8)255, (uint8)52,  (uint8)41 );
    Colour levelBackGreen = Colour( (uint8)10,  (uint8)87,  (uint8)9 );
    Colour levelGreen     = Colour( (uint8)27,  (uint8)255, (uint8)23 );
    
    if (levelClipping)
        g.setColour( clippingRed );     // Bright Red
    else
        g.setColour ( clipBackRed );    // Dark Red
    
    g.fillRect  ( leftChannelClipBack );
    g.fillRect  ( rightChannelClipBack );
    
    g.setColour ( levelBackGreen );     // Dark Green
    g.fillRect  ( leftChannelBack );
    g.fillRect  ( rightChannelBack );
    
    g.setColour ( levelGreen );
    g.fillRect  ( leftChannelLevel );
    g.fillRect  ( rightChannelLevel );
}

void OutMeter::resized()
{
    int reducer = 2;
    auto totalArea = getLocalBounds();
    
    Rectangle<int> reducedArea   = totalArea.reduced         ( reducer );
    Rectangle<int> clipArea      = reducedArea.removeFromTop ( reducedArea.getHeight() * 0.2f );
    Rectangle<int> clipLeftArea  = clipArea.removeFromLeft   ( clipArea.getWidth() * 0.5f ).reduced( reducer );
    Rectangle<int> clipRightArea = clipArea.reduced          ( reducer );
    
    leftChannelClipBack.setBounds  ( clipLeftArea.getX(), clipLeftArea.getY(),
                                     clipLeftArea.getWidth(), clipLeftArea.getHeight() );
    rightChannelClipBack.setBounds ( clipRightArea.getX(), clipRightArea.getY(),
                                     clipRightArea.getWidth(), clipRightArea.getHeight() );
    
    
    Rectangle<int> meterLeftArea  = reducedArea.removeFromLeft( reducedArea.getWidth() * 0.5f ).reduced( reducer );
    Rectangle<int> meterRightArea = reducedArea.reduced( reducer );
    
    leftChannelBack.setBounds   ( meterLeftArea.getX(), meterLeftArea.getY(),
                                  meterLeftArea.getWidth(), meterLeftArea.getHeight() );
    
    leftChannelLevel.setBounds  ( meterLeftArea.getX(), meterLeftArea.getY() + meterLeftArea.getHeight(),
                                  meterLeftArea.getWidth(), -meterLeftArea.getHeight() * heightMult );
    
    rightChannelBack.setBounds  ( meterRightArea.getX(), meterRightArea.getY(),
                                  meterRightArea.getWidth(), meterRightArea.getHeight() );
    
    rightChannelLevel.setBounds ( meterRightArea.getX(), meterRightArea.getY() + meterRightArea.getHeight(),
                                  meterRightArea.getWidth(), -meterRightArea.getHeight() * heightMult );
}


void OutMeter::outMeterLevel(float level, float sampleRate)
{
    float multiplier = (level < 1.0f) ? level : 1.0f;
    
    SR = (SR != sampleRate) ? sampleRate : SR;
    
    heightMultiplier(multiplier);
    
    levelClipping  = (level < 1.0f) ? false : true;
    
    resized();
    repaint();
}


void OutMeter::heightMultiplier(float mult)
{
    if (mult > heightMult)
        heightMult = mult * ( 1.0f - (1.0f / decayFactorRise) );
    else
        heightMult *= 1.0f - (1.0f / decayFactorFall);
    
    if (mult == 1.0f)
        heightMult = 1.0f;
}
