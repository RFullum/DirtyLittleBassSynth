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
    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    DLBS::SetupSlider(this, cutoffSlider, juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    DLBS::SetupSlider(this, resSlider,    juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    cutoffSlider.setLookAndFeel(res.dialLookAndFeel);
    cutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    resSlider   .setLookAndFeel(res.dialLookAndFeel);

    DLBS::SetupSectionLabel(this, sectionLabel, "Filter", res.theme.textSecondary);

    DLBS::SetupLabel(this, cutoffLabel, "Cutoff", txt, 15.0f);
    DLBS::SetupLabel(this, resLabel,    "Rez",    txt, 15.0f);

    filterType.Setup(*res.apvts
                     , "filter_type"
                     , juce::StringArray({"-12", "-24", "-48", "Notch"})
                     , accent
                     , res.theme.structure
                     , res.theme.textSecondary);
    addAndMakeVisible(filterType);

    cutoffAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filter_cutoff", cutoffSlider);
    resAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filter_res",    resSlider);

    DLBS::SetSliderTextFormat(cutoffSlider, DLBS::FormatCutoffHz);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    filterVisual.setColors(accent, bg, bgFade);

    addAndMakeVisible(filterVisual);
}

void FilterPanel::resized()
{
    static constexpr int sectionSpacerSize = 2;
    static constexpr int rowSize           = 20;
    static constexpr int leftMargin        = 45;
    
    auto bounds = getLocalBounds().reduced(sectionSpacerSize);
    sectionLabel.setBounds(bounds.removeFromTop(16));
    
    auto resArea = bounds.removeFromRight(50);
    resArea   .removeFromBottom(4);
    resLabel  .setBounds(resArea.removeFromTop(15));
    resSlider .setBounds(resArea);
    filterType.setBounds(bounds.removeFromBottom(rowSize)
                               .reduced(sectionSpacerSize, sectionSpacerSize));
    
    auto cutoffArea = bounds.removeFromBottom(rowSize);
    cutoffLabel .setBounds(cutoffArea.removeFromLeft(leftMargin));
    cutoffSlider.setBounds(cutoffArea);
    
    bounds.removeFromLeft(leftMargin);
    filterVisual.setBounds(bounds);
}

void FilterPanel::Update()
{
    filterVisual.drawFilterShape(filterType.GetSelectedIndex() + 1
                                 , (float)cutoffSlider.getValue()
                                 , (float)resSlider   .getValue());
}
