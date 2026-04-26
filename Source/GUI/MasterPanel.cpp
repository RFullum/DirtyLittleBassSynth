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

    SetupSlider(this, masterGainSlider, juce::Slider::SliderStyle::LinearVertical, res.orangePeel, res.orangePeel, res.textColor, true);
    SetupLabel (this, masterGainLabel,  "Out Gain", res.textColor, 16.0f);

    masterGainSlider.setLookAndFeel(res.dialLookAndFeel);

    gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "master_gain", masterGainSlider);

    outMeter.setColors(res.magicMint, res.fieryRose);
    addAndMakeVisible(outMeter);
}

void MasterPanel::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    auto area = getLocalBounds().toFloat();

    g.setGradientFill(juce::ColourGradient::vertical(resources.onyx,      area.getHeight() * 0.59f
                                                     , resources.magicMint, area.getHeight()));
    g.fillRoundedRectangle(area, cornerRound);
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

    auto meterArea = area.removeFromBottom(area.getHeight() / 2);
    auto labelArea = area.removeFromTop(mainLabelHeight);

    masterGainLabel .setBounds(labelArea);
    masterGainSlider.setBounds(area);
    outMeter        .setBounds(meterArea);
}
