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

    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    SetupSlider(this, cutoffSlider, juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    SetupSlider(this, resSlider,    juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    cutoffSlider.setLookAndFeel(res.dialLookAndFeel);
    resSlider   .setLookAndFeel(res.dialLookAndFeel);

    SetupSectionLabel(this, sectionLabel, "Filter", res.theme.textSecondary);

    SetupLabel(this, cutoffLabel, "Cutoff", txt, 15.0f);
    SetupLabel(this, resLabel,    "Rez",    txt, 15.0f);

    SetupComboBox(this, filterType, juce::StringArray({"-12LPF", "-24LPF", "-48LPF", "Notch"}));

    cutoffAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "filter_cutoff", cutoffSlider);
    resAtt        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "filter_res",    resSlider);
    filterTypeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(*res.apvts, "filter_type",   filterType);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    filterVisual.setColors(accent, res.theme.structure, bg, bgFade);

    addAndMakeVisible(filterVisual);
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

    auto area = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(area.removeFromTop(16).reduced(8, 0));

    auto reduced = area.reduced(sectionSpacerSize);

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
