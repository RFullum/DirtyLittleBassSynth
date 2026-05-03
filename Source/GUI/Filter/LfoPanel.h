/*
  ==============================================================================

    LfoPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OscVisual.h"
#include "SegmentedControl.h"

//============================================================

/// Bottom-right section: filter LFO shape (with visual) plus rate (free-run Hz
/// or tempo-synced subdivision) and amount sliders.
/// FRQ mode shows the Hz slider; SYNC mode swaps it for a discrete subdivision
/// selector that locks to host transport via the per-block snapshot in
/// BassSynthVoice::PrepareDspForBlock.
class LfoPanel
    : public  juce::Component
    , private juce::AudioProcessorValueTreeState::Listener
{
public:
    LfoPanel(GuiResources &res);
    ~LfoPanel() override;

    void paint(juce::Graphics &) override;
    void resized() override;

    /// Called from the editor's timer so the LFO shape visual rebuilds its path.
    void Update();

private:
    /// Listener callback — fires on user toggle of FRQ/SYNC and on host automation.
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    /// Toggles visibility of lfoFreqSlider vs lfoSyncDivSlider and updates the
    /// "Freq" / "Sync" label based on the current sync mode.
    void RefreshSyncModeLook();

    GuiResources &resources;

    OscVisual lfoVisual;

    juce::Slider     lfoShapeSlider;
    juce::Slider     lfoFreqSlider;       // visible in FRQ mode
    juce::Slider     lfoSyncDivSlider;    // visible in SYNC mode
    juce::Slider     lfoAmountSlider;

    SegmentedControl lfoSyncControl;      // FRQ / SYNC toggle

    juce::Label sectionLabel;
    juce::Label lfoRateLabel;             // "Freq" / "Sync"
    juce::Label lfoAmountLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shapeAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> divAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAtt;

    juce::Rectangle<int> syncRowRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoPanel)
};
