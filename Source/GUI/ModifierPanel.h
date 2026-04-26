/*
  ==============================================================================

    ModifierPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//============================================================

/// Top-right section: Ring Mod / Frequency Shifter / Sample & Hold knob rows
/// with shared Tone / Pitch / Dry-Wet column headers.
class ModifierPanel
    : public juce::Component
{
public:
    ModifierPanel(GuiResources &res);

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    GuiResources &resources;

    juce::Slider ringToneSlider;
    juce::Slider ringPitchSlider;
    juce::Slider ringDryWetSlider;
    juce::Slider frqShftPitchSlider;
    juce::Slider frqShftDryWetSlider;
    juce::Slider sHPitchSlider;
    juce::Slider sHDryWetSlider;

    juce::Label ringLabel;
    juce::Label frqShftLabel;
    juce::Label sHLabel;
    juce::Label toneLabel;
    juce::Label pitchLabel;
    juce::Label dryWetLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringToneAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringPitchAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringDryWetAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frqShftPitchAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frqShftDryWetAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sHPitchAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sHDryWetAtt;

    juce::Rectangle<float> ringRowBg;
    juce::Rectangle<float> frqShftRowBg;
    juce::Rectangle<float> sAndHRowBg;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModifierPanel)
};
