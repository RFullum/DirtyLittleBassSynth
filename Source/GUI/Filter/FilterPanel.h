/*
  ==============================================================================

    FilterPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "FilterVisual.h"
#include "SegmentedControl.h"

//============================================================

/// Bottom-left section: filter visualizer + cutoff/resonance sliders + filter type combo.
class FilterPanel
    : public juce::Component
{
public:
    FilterPanel(GuiResources &res);

    void resized() override;

    /// Called from the editor's timer to redraw the filter response curve from
    /// the current cutoff / resonance / type values.
    void Update();

private:
    GuiResources &resources;

    FilterVisual filterVisual;

    juce::Slider     cutoffSlider;
    juce::Slider     resSlider;
    SegmentedControl filterType;

    juce::Label sectionLabel;
    juce::Label cutoffLabel;
    juce::Label resLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resAtt;

    // Cached APVTS raw-value pointers for modulation amounts. Read each timer
    // tick to compute the effective (modulated) cutoff/res positions for the
    // animated visualiser.
    std::atomic<float> *envCOAmtPtr  = nullptr;
    std::atomic<float> *envResAmtPtr = nullptr;
    std::atomic<float> *lfoAmtPtr    = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};
