/*
  ==============================================================================

    ModifierPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "DryWetSlider.h"

//============================================================

/// Top-right section: Portamento + Foldback knobs (with portamento ON/OFF and
/// ALWAYS/LEGATO mode buttons), plus Ring Mod / Frequency Shifter / Sample &
/// Hold knob rows with shared Tone / Pitch / Dry-Wet column headers.
class ModifierPanel
    : public  juce::Component
    , private juce::AudioProcessorValueTreeState::Listener
{
public:
    ModifierPanel(GuiResources &res);
    ~ModifierPanel() override;

    void resized() override;

private:
    /// Listener fires on user click + automation changes for porta_on / porta_legato.
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    /// Updates portaSlider / portaLabel alpha (1.0 on, 0.5 off) and refreshes
    /// the two button labels to reflect the current toggle state.
    void RefreshPortaLook();

    GuiResources &resources;

    juce::Slider portaSlider;
    juce::Slider foldbackSlider;
    juce::Slider ringToneSlider;
    juce::Slider ringPitchSlider;
    DryWetSlider ringDryWetSlider;
    juce::Slider frqShftPitchSlider;
    DryWetSlider frqShftDryWetSlider;
    juce::Slider sHPitchSlider;
    DryWetSlider sHDryWetSlider;

    juce::TextButton portaOnButton;
    juce::TextButton portaLegatoButton;

    juce::Label sectionLabel;
    juce::Label portaLabel;
    juce::Label foldbackLabel;
    juce::Label ringLabel;
    juce::Label frqShftLabel;
    juce::Label sHLabel;
    juce::Label toneLabel;
    juce::Label pitchLabel;
    juce::Label dryWetLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> portaAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> foldbackAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringToneAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringPitchAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringDryWetAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frqShftPitchAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frqShftDryWetAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sHPitchAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sHDryWetAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> portaOnAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> portaLegatoAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModifierPanel)
};
