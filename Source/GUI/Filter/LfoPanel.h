/*
  ==============================================================================

    LfoPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OscVisual.h"

//============================================================

/// Bottom-right section: filter LFO shape (with visual) plus freq and amount sliders.
class LfoPanel
    : public juce::Component
{
public:
    LfoPanel(GuiResources &res);

    void paint(juce::Graphics &) override;
    void resized() override;

    /// Called from the editor's timer so the LFO shape visual rebuilds its path.
    void Update();

private:
    GuiResources &resources;

    OscVisual lfoVisual;

    juce::Slider lfoShapeSlider;
    juce::Slider lfoFreqSlider;
    juce::Slider lfoAmountSlider;

    juce::Label sectionLabel;
    juce::Label lfoFreqLabel;
    juce::Label lfoAmountLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shapeAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAtt;

    // Reserved space for the future ms/sync toggle + time display.
    // Drawn as decorative placeholders for now (no interaction).
    juce::Rectangle<int> syncFrqRect;
    juce::Rectangle<int> syncSyncRect;
    juce::Rectangle<int> syncDisplayRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoPanel)
};
