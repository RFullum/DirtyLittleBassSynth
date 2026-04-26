/*
  ==============================================================================

    FilterAdsrPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//============================================================

/// Bottom-middle section: filter ADSR sliders plus envelope-to-cutoff and
/// envelope-to-resonance amount knobs.
class FilterAdsrPanel
    : public juce::Component
{
public:
    FilterAdsrPanel(GuiResources &res);

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    GuiResources &resources;

    juce::Slider fltAttackSlider;
    juce::Slider fltDecaySlider;
    juce::Slider fltSustainSlider;
    juce::Slider fltReleaseSlider;
    juce::Slider adsrToCutoffSlider;
    juce::Slider adsrToResSlider;

    juce::Label fltAttackLabel;
    juce::Label fltDecayLabel;
    juce::Label fltSustainLabel;
    juce::Label fltReleaseLabel;
    juce::Label adsrToCutoffLabel;
    juce::Label adsrToResLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toCutoffAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toResAtt;

    juce::Rectangle<float> innerBg;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterAdsrPanel)
};
