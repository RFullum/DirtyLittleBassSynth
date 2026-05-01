/*
  ==============================================================================

    MasterColumn.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OutMeter.h"

//============================================================

/// Right-column section: master gain slider + output meter.
class MasterColumn
    : public juce::Component
{
public:
    MasterColumn(GuiResources &res);

    void paint(juce::Graphics &) override;
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

    // Reserved space for the future output scope (audio thread -> UI ring buffer).
    // Drawn as a decorative outlined rectangle for now.
    juce::Rectangle<int> scopeRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterColumn)
};
