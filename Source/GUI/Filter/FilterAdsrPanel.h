/*
  ==============================================================================

    FilterAdsrPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "AdsrVisual.h"

//==============================================================================

class FilterAdsrPanel
    : public juce::Component
{
public:
    FilterAdsrPanel(GuiResources &res);

    void resized() override;

    void Update();

private:
    AdsrVisual adsrVisual;

    juce::Slider fltAttackSlider;
    juce::Slider fltDecaySlider;
    juce::Slider fltSustainSlider;
    juce::Slider fltReleaseSlider;
    juce::Slider adsrToCutoffSlider;
    juce::Slider adsrToResSlider;

    juce::Label sectionLabel;
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterAdsrPanel)
};
