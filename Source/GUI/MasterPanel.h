/*
  ==============================================================================

    MasterPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OutMeter.h"

//============================================================

/// Right-column section: master gain slider + output meter.
class MasterPanel
    : public juce::Component
{
public:
    MasterPanel(GuiResources &res);

    void resized() override;

    /// Pushes the latest L/R output magnitudes into the meter. Called from the editor's timer.
    void Update(float leftLevel, float rightLevel, float sampleRate);

private:
    GuiResources &resources;

    juce::Label  sectionLabel;
    juce::Slider masterGainSlider;
    juce::Label  masterGainLabel;
    OutMeter     outMeter;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterPanel)
};
