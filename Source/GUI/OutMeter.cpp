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
                       decayFactorRise(decayRateRise * SR), decayFactorFall(decayRateFall * SR),
                       clippingRed(juce::Colour( (juce::uint8)255, (juce::uint8)10, (juce::uint8)27, (juce::uint8)255 ) )
{}

OutMeter::~OutMeter() {}


void OutMeter::setColors(juce::Colour &levelColor, juce::Colour &clipColor)
{
    clipBackRed    = clipColor.darker().darker();
    clippingRed    = clipColor;
    levelBackGreen = levelColor.darker().darker();
    levelGreen     = levelColor.brighter().brighter();
}

void OutMeter::paint(juce::Graphics& g)
{
    g.setColour(levelClipping ? clippingRed : clipBackRed);
    g.fillRect(leftChannelClipBack);
    g.fillRect(rightChannelClipBack);

    g.setColour(levelBackGreen);
    g.fillRect(leftChannelBack);
    g.fillRect(rightChannelBack);

    g.setColour(levelGreen);
    g.fillRect(leftChannelLevel);
    g.fillRect(rightChannelLevel);
}

void OutMeter::resized()
{
    int reducer = 2;
    auto totalArea = getLocalBounds();
    
    juce::Rectangle<int> reducedArea   = totalArea.reduced         ( reducer );
    juce::Rectangle<int> clipArea      = reducedArea.removeFromTop ( reducedArea.getHeight() * 0.2f );
    juce::Rectangle<int> clipLeftArea  = clipArea.removeFromLeft   ( clipArea.getWidth() * 0.5f ).reduced( reducer );
    juce::Rectangle<int> clipRightArea = clipArea.reduced          ( reducer );
    
    leftChannelClipBack.setBounds  ( clipLeftArea.getX(), clipLeftArea.getY(),
                                     clipLeftArea.getWidth(), clipLeftArea.getHeight() );
    rightChannelClipBack.setBounds ( clipRightArea.getX(), clipRightArea.getY(),
                                     clipRightArea.getWidth(), clipRightArea.getHeight() );
    
    
    juce::Rectangle<int> meterLeftArea  = reducedArea.removeFromLeft( reducedArea.getWidth() * 0.5f ).reduced( reducer );
    juce::Rectangle<int> meterRightArea = reducedArea.reduced( reducer );
    
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

    // TODO: should also recompute decayFactorRise/decayFactorFall when SR changes
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
