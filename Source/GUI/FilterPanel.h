/*
  ==============================================================================

    FilterPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "FilterVisual.h"

//============================================================

/// Bottom-left section: filter visualizer + cutoff/resonance sliders + filter type combo.
class FilterPanel
    : public juce::Component
{
public:
    FilterPanel(GuiResources &res);

    void paint(juce::Graphics &g) override;
    void resized() override;

    /// Called from the editor's timer to redraw the filter response curve from
    /// the current cutoff / resonance / type values.
    void Update();

private:
    GuiResources &resources;

    FilterVisual filterVisual;

    juce::Slider   cutoffSlider;
    juce::Slider   resSlider;
    juce::ComboBox filterType;

    juce::Label cutoffLabel;
    juce::Label resLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   cutoffAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   resAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAtt;

    juce::Rectangle<float> innerBg;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};
