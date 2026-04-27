/*
  ==============================================================================

    OutMeter.h
    Created: 10 Jan 2021 2:32:52pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OutMeter  : public juce::Component
{
public:
    OutMeter();
    ~OutMeter() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void outMeterLevel(float level, float sampleRate);
    
    
    /// Sets the colors of the level and clipping meter
    void setColors(juce::Colour levelColor, juce::Colour clipColor);

private:
    void heightMultiplier(float mult);
    
    juce::Rectangle<int> leftChannelBack;
    juce::Rectangle<int> leftChannelClipBack;
    juce::Rectangle<int> rightChannelBack;
    juce::Rectangle<int> rightChannelClipBack;
    
    juce::Rectangle<int> leftChannelLevel;
    juce::Rectangle<int> rightChannelLevel;
    
    float outLevel;
    bool levelClipping;
    
    float heightMult;
    float SR;
    float decayRateRise;        // = 0.0005f;
    float decayRateFall;        // = 0.001f; // in ms... in Seconds?
    float decayFactorRise;
    float decayFactorFall;      // for N in value *= (1 - 1/N), in samples
    
    
    // Colors
    juce::Colour clipBackRed;
    juce::Colour clippingRed;
    juce::Colour levelBackGreen;
    juce::Colour levelGreen;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutMeter)
};
