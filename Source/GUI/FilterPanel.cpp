/*
  ==============================================================================

    FilterPanel.cpp

  ==============================================================================
*/

#include "FilterPanel.h"
#include "GuiHelpers.h"

//============================================================

FilterPanel::FilterPanel(GuiResources &res)
: resources(res)
{
    using namespace dlbs;

    SetupSlider(this, cutoffSlider, juce::Slider::SliderStyle::LinearHorizontal, res.magicMint, res.magicMint, res.textColor, false);
    SetupSlider(this, resSlider,    juce::Slider::SliderStyle::LinearVertical,   res.magicMint, res.magicMint, res.textColor, false);

    cutoffSlider.setLookAndFeel(res.dialLookAndFeel);
    resSlider   .setLookAndFeel(res.dialLookAndFeel);

    SetupLabel(this, cutoffLabel, "Cutoff", res.textColor, 15.0f);
    SetupLabel(this, resLabel,    "Rez",    res.textColor, 15.0f);

    SetupComboBox(this, filterType, juce::StringArray({"-12LPF", "-24LPF", "-48LPF", "Notch"}));

    cutoffAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "filter_cutoff", cutoffSlider);
    resAtt        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "filter_res",    resSlider);
    filterTypeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(*res.apvts, "filter_type",   filterType);

    juce::Colour onyxDark = res.onyx.darker().darker();
    filterVisual.setColors(res.magicMint, onyxDark, res.onyx, onyxDark);

    addAndMakeVisible(filterVisual);
}

void FilterPanel::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    g.setColour(resources.magicMint);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRound);

    g.setColour(resources.onyx);
    g.fillRoundedRectangle(innerBg, cornerRound);
}

void FilterPanel::Update()
{
    filterVisual.drawFilterShape(filterType.getSelectedId()
                                 , (float) cutoffSlider.getValue()
                                 , (float) resSlider   .getValue());
}

void FilterPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int filtLabelHeight   = 30;
    constexpr int morphLabelWidth   = 50;
    constexpr int filterTypeWidth   = 75;

    auto area    = getLocalBounds().reduced(sectionSpacerSize);
    auto reduced = area.reduced(sectionSpacerSize);
    innerBg = reduced.toFloat();

    int resWidth = reduced.getWidth() / 8;

    auto resArea         = reduced.removeFromRight (resWidth);
    auto resLabelArea    = resArea.removeFromTop   (filtLabelHeight);
    auto coLabelArea     = reduced.removeFromLeft  (morphLabelWidth);
    auto fltTypeArea     = reduced.removeFromBottom(filtLabelHeight).removeFromRight(filterTypeWidth);
    auto fltCOSliderArea = reduced.removeFromBottom(filtLabelHeight);

    resLabel    .setBounds(resLabelArea);
    resSlider   .setBounds(resArea);
    filterType  .setBounds(fltTypeArea);
    cutoffLabel .setBounds(coLabelArea.removeFromBottom(coLabelArea.getHeight() / 2));
    cutoffSlider.setBounds(fltCOSliderArea);
    filterVisual.setBounds(reduced);
}
