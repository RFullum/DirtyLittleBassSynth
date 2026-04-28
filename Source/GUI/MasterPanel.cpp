/*
  ==============================================================================

    MasterPanel.cpp

  ==============================================================================
*/

#include "MasterPanel.h"
#include "GuiHelpers.h"

//============================================================

MasterPanel::MasterPanel(GuiResources &res)
: resources(res)
{
    using namespace dlbs;

    auto accent = res.theme.primaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    SetupSectionLabel(this, sectionLabel, "Master", res.theme.textSecondary);

    SetupSlider(this, masterGainSlider, juce::Slider::SliderStyle::LinearVertical, accent, thumb, txt);
    SetupLabel (this, masterGainLabel,  "Out Gain", txt, 16.0f);

    masterGainSlider.setLookAndFeel(res.dialLookAndFeel);

    gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "master_gain", masterGainSlider);

    outMeter.setColors(accent, res.theme.pinkAccent);
    addAndMakeVisible(outMeter);
}

void MasterPanel::Update(float outLevel, float sampleRate)
{
    outMeter.outMeterLevel(outLevel, sampleRate);
}

void MasterPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int mainLabelHeight   = 30;

    auto area = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(area.removeFromTop(16).reduced(8, 0));

    auto meterArea = area.removeFromBottom(area.getHeight() / 2);
    auto labelArea = area.removeFromTop(mainLabelHeight);

    masterGainLabel .setBounds(labelArea);
    masterGainSlider.setBounds(area);
    outMeter        .setBounds(meterArea);
}
