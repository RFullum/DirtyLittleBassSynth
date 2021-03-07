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
class OutMeter  : public Component
{
public:
    OutMeter();
    ~OutMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void outMeterLevel(float level, float sampleRate);

private:
    void heightMultiplier(float mult);
    
    Rectangle<int> leftChannelBack;
    Rectangle<int> leftChannelClipBack;
    Rectangle<int> rightChannelBack;
    Rectangle<int> rightChannelClipBack;
    
    Rectangle<int> leftChannelLevel;
    Rectangle<int> rightChannelLevel;
    
    float outLevel;
    bool levelClipping;
    
    float heightMult;
    float SR;
    float decayRateRise;        // = 0.0005f;
    float decayRateFall;        // = 0.001f; // in ms... in Seconds?
    float decayFactorRise;
    float decayFactorFall;      // for N in value *= (1 - 1/N), in samples
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutMeter)
};
