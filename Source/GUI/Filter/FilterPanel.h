/*
  ==============================================================================

    FilterPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "FilterVisual.h"
#include "GUI/Selector.h"

//==============================================================================

class FilterPanel
    : public juce::Component
{
public:
    FilterPanel(GuiResources &res);

    void resized() override;

    void Update();

private:
    GuiResources &resources;

    FilterVisual filterVisual;

    juce::Slider     cutoffSlider;
    juce::Slider     resSlider;
    Selector         filterType;

    juce::Label sectionLabel;
    juce::Label cutoffLabel;
    juce::Label resLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resAtt;

    // Cached raw-value pointers read each timer tick for the modulated visualiser.
    std::atomic<float> *envCOAmtPtr  = nullptr;
    std::atomic<float> *envResAmtPtr = nullptr;
    std::atomic<float> *lfoAmtPtr    = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};
