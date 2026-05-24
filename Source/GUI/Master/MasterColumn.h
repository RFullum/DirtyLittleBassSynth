/*
  ==============================================================================

    MasterColumn.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OutMeter.h"
#include "ScopeVisual.h"

//==============================================================================

// TODO: Re-organize layout after all features hooked into audio.

class MasterColumn
    : public  juce::Component
    , private juce::AudioProcessorValueTreeState::Listener
{
public:
    MasterColumn(GuiResources &res);
    ~MasterColumn() override;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update(float leftLevel, float rightLevel, float gainReductionDb, float sampleRate);

private:
    void RefreshCeilingEnabledLook();
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    GuiResources &resources;

    juce::Label sectionLabel;

    juce::Slider masterGainSlider;
    juce::Label  masterGainLabel;

    juce::TextButton ceilingOnButton;
    juce::Slider     ceilingSlider;
    juce::Label      ceilingLabel;

    juce::Slider wideSlider;
    juce::Label  wideLabel;

    juce::Slider monoCrossoverSlider;
    juce::Label  monoCrossoverLabel;

    OutMeter                       outMeter;
    std::unique_ptr<ScopeVisual>   scopeVisual;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wideAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> monoCrossoverAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ceilingAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ceilingOnAtt;

    juce::Rectangle<int> grMeterRect;

    // Latest GR value (dB, >= 0). Written by Update() from the audio-thread feed.
    float gainReductionDb = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterColumn)
};
