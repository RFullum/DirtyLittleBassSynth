/*
  ==============================================================================

    AmpAdsrPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//============================================================

/// Top-middle section: amp ADSR sliders plus portamento and foldback distortion knobs.
class AmpAdsrPanel
    : public juce::Component
{
public:
    AmpAdsrPanel(GuiResources &res);

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    GuiResources &resources;

    juce::Slider oscAttackSlider;
    juce::Slider oscDecaySlider;
    juce::Slider oscSustainSlider;
    juce::Slider oscReleaseSlider;
    juce::Slider portaSlider;
    juce::Slider foldbackSlider;

    juce::Label oscAttackLabel;
    juce::Label oscDecayLabel;
    juce::Label oscSustainLabel;
    juce::Label oscReleaseLabel;
    juce::Label portaLabel;
    juce::Label foldbackLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> portaAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> foldbackAtt;

    juce::Rectangle<float> rotaryBg;
    juce::Rectangle<float> slidersBg;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpAdsrPanel)
};
