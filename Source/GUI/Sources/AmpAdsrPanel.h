/*
  ==============================================================================

    AmpAdsrPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "AdsrVisual.h"

//============================================================

/// Lower half of the Sources column: amp ADSR sliders + envelope visual.
class AmpAdsrPanel
    : public juce::Component
{
public:
    AmpAdsrPanel(GuiResources &res);

    void resized() override;

    /// Called from the editor's timer so the envelope visual rebuilds its path.
    void Update();

private:
    AdsrVisual adsrVisual;

    juce::Slider oscAttackSlider;
    juce::Slider oscDecaySlider;
    juce::Slider oscSustainSlider;
    juce::Slider oscReleaseSlider;

    juce::Label sectionLabel;
    juce::Label oscAttackLabel;
    juce::Label oscDecayLabel;
    juce::Label oscSustainLabel;
    juce::Label oscReleaseLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpAdsrPanel)
};
