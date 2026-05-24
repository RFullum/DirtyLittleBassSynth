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

//==============================================================================

class LfoPanel
    : public  juce::Component
    , private juce::AudioProcessorValueTreeState::Listener
{
public:
    LfoPanel(GuiResources &res);
    ~LfoPanel() override;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Update();

private:
    void parameterChanged(const juce::String &parameterID, float newValue) override;
    void RefreshSyncModeLook();

    GuiResources &resources;

    OscVisual lfoVisual;

    juce::Slider     lfoShapeSlider;
    juce::Slider     lfoFreqSlider;       // visible in FRQ mode
    juce::Slider     lfoSyncDivSlider;    // visible in SYNC mode
    juce::Slider     lfoAmountSlider;

    SegmentedControl lfoSyncControl;

    juce::Label sectionLabel;
    juce::Label lfoRateLabel;             // "Freq" or "Sync" depending on mode
    juce::Label lfoAmountLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shapeAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> divAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAtt;

    juce::Rectangle<int> syncRowRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoPanel)
};
