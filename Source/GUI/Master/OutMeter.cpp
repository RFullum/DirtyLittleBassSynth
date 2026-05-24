/*
  ==============================================================================

    OutMeter.cpp
    Created: 10 Jan 2021 2:32:52pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OutMeter.h"

//==============================================================================

OutMeter::OutMeter()
: clipLitColor  (juce::Colour((juce::uint8)255, (juce::uint8)10, (juce::uint8)27))
, clipDimColor  (juce::Colour((juce::uint8)50,  (juce::uint8)10, (juce::uint8)15))
, levelColor    (juce::Colour((juce::uint8)0,   (juce::uint8)200,(juce::uint8)180))
, levelBackColor(juce::Colour((juce::uint8)20,  (juce::uint8)25, (juce::uint8)35))
{
    setOpaque(false);
}

void OutMeter::paint(juce::Graphics &g)
{
    constexpr float corner = 1.5f;

    g.setColour(leftClipping ? clipLitColor : clipDimColor);
    g.fillRoundedRectangle(leftClipRect.toFloat(), corner);

    g.setColour(rightClipping ? clipLitColor : clipDimColor);
    g.fillRoundedRectangle(rightClipRect.toFloat(), corner);

    g.setColour(levelBackColor);
    g.fillRoundedRectangle(leftMeterBack .toFloat(), corner);
    g.fillRoundedRectangle(rightMeterBack.toFloat(), corner);

    g.setColour(levelColor);
    g.fillRoundedRectangle(leftMeterLevel .toFloat(), corner);
    g.fillRoundedRectangle(rightMeterLevel.toFloat(), corner);
}

void OutMeter::resized()
{
    constexpr int reducer        = 2;
    constexpr int clipStripHeight = 5;
    constexpr int clipGap         = 2;

    auto bounds      = getLocalBounds();
    auto reducedArea = bounds.reduced(reducer);

    auto clipArea = reducedArea.removeFromTop(clipStripHeight);
    reducedArea.removeFromTop(clipGap);

    auto clipLeft  = clipArea.removeFromLeft(clipArea.getWidth() / 2).reduced(reducer, 0);
    auto clipRight = clipArea                                         .reduced(reducer, 0);

    leftClipRect  = clipLeft;
    rightClipRect = clipRight;

    auto meterLeftArea  = reducedArea.removeFromLeft(reducedArea.getWidth() / 2).reduced(reducer, 0);
    auto meterRightArea = reducedArea                                            .reduced(reducer, 0);

    leftMeterBack  = meterLeftArea;
    rightMeterBack = meterRightArea;

    const int leftMeterH  = meterLeftArea .getHeight();
    const int rightMeterH = meterRightArea.getHeight();
    const int leftFillH   = (int)(leftMeterH  * juce::jlimit(0.0f, 1.0f, leftHeightMult));
    const int rightFillH  = (int)(rightMeterH * juce::jlimit(0.0f, 1.0f, rightHeightMult));

    leftMeterLevel.setBounds (meterLeftArea.getX(),
                              meterLeftArea.getY()  + leftMeterH  - leftFillH,
                              meterLeftArea.getWidth(),
                              leftFillH);

    rightMeterLevel.setBounds(meterRightArea.getX(),
                              meterRightArea.getY() + rightMeterH - rightFillH,
                              meterRightArea.getWidth(),
                              rightFillH);
}

void OutMeter::outMeterLevel(float leftLevel, float rightLevel, float sampleRate)
{
    if (sampleRate > 0.0f && SR != sampleRate)
    {
        SR              = sampleRate;
        decayFactorRise = decayRateRise * SR;
        decayFactorFall = decayRateFall * SR;
    }

    leftClipping  = leftLevel  >= 1.0f;
    rightClipping = rightLevel >= 1.0f;

    updateHeight(leftHeightMult,  juce::jmin(leftLevel,  1.0f));
    updateHeight(rightHeightMult, juce::jmin(rightLevel, 1.0f));

    resized();
    repaint();
}

void OutMeter::setColors(juce::Colour level, juce::Colour clip)
{
    levelColor     = level;
    levelBackColor = level.darker().darker();
    clipLitColor   = clip;
    clipDimColor   = clip.darker().darker();
}

void OutMeter::updateHeight(float &heightMult, float magnitude)
{
    if (magnitude > heightMult)
        heightMult = magnitude * (1.0f - (1.0f / decayFactorRise));
    else
        heightMult *= 1.0f - (1.0f / decayFactorFall);

    if (magnitude == 1.0f)
        heightMult = 1.0f;
}
