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

// TODO: Re-organize layout after all features hooked into audio.

/// Right-hand column. Top-to-bottom: Out Gain + Limiter (on/off + Ceiling rotary),
/// scope, output meter with adjacent vertical gain-reduction meter, Haas widener,
/// bass mono-izer crossover.
/// The widener / ceiling / mono-izer / GR meter are visual placeholders for now —
/// no APVTS bindings and no DSP yet. Hook them up as the processors land.
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
    /// Updates the alpha of the ceiling rotary + label to reflect the on/off state.
    void RefreshCeilingEnabledLook();

    GuiResources &resources;

    juce::Label sectionLabel;

    juce::Slider masterGainSlider;
    juce::Label  masterGainLabel;

    // Limiter (placeholder — no DSP yet)
    juce::TextButton ceilingOnButton;
    juce::Slider     ceilingSlider;
    juce::Label      ceilingLabel;

    // Haas widener (placeholder — no DSP yet). Bipolar horizontal slider, center = mono.
    juce::Slider wideSlider;
    juce::Label  wideLabel;

    // Bass mono-izer crossover (placeholder — no DSP yet).
    juce::Slider monoCrossoverSlider;
    juce::Label  monoCrossoverLabel;

    OutMeter outMeter;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAtt;

    // Reserved space for the future output scope (audio thread -> UI ring buffer).
    juce::Rectangle<int> scopeRect;

    // Reserved space for the future limiter gain-reduction meter — vertical bar
    // sitting next to the output meter, fills downward as GR increases.
    juce::Rectangle<int> grMeterRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterColumn)
};
