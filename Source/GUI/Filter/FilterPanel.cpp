/*
  ==============================================================================

    FilterPanel.cpp

  ==============================================================================
*/

#include "FilterPanel.h"
#include "GuiHelpers.h"
#include "GUI/GuiHelpers.h"
#include "GUI/Format.h"

//==============================================================================

FilterPanel::FilterPanel(GuiResources &res)
: resources(res)
, filterVisual(res.theme.secondaryAccent
               , res.theme.background
               , res.theme.background.darker())
, filterType(*res.apvts
             , "filter_type"
             , res.theme
             , juce::StringArray({"-12","-24","-48","Notch"})
             , juce::FontOptions("Helvetica", 12.0f, juce::Font::bold))
{
    setOpaque(false);

    envCOAmtPtr  = res.apvts->getRawParameterValue("filtEnv_COAmt");
    envResAmtPtr = res.apvts->getRawParameterValue("filtEnv_ResAmt");
    lfoAmtPtr    = res.apvts->getRawParameterValue("filtLFO_amt");
    
    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    GuiHelpers::SetupSlider(this, cutoffSlider, juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    GuiHelpers::SetupSlider(this, resSlider,    juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    cutoffSlider.setLookAndFeel(res.dialLookAndFeel);
    cutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    resSlider   .setLookAndFeel(res.dialLookAndFeel);

    GuiHelpers::SetupSectionLabel(this, sectionLabel, "Filter", res.theme.textSecondary);

    GuiHelpers::SetupLabel(this, cutoffLabel, "Cutoff", txt, 15.0f);
    GuiHelpers::SetupLabel(this, resLabel,    "Rez",    txt, 15.0f);

    filterType.getProperties().set("paramID", "filter_type");
    addAndMakeVisible(filterType);

    cutoffAtt = DLBS::AttachSlider(*res.apvts, "filter_cutoff", cutoffSlider);
    resAtt    = DLBS::AttachSlider(*res.apvts, "filter_res",    resSlider);

    Format::SetSliderTextFormat(cutoffSlider, DLBS::FormatCutoffHz);

    addAndMakeVisible(filterVisual);

    GuiHelpers::SetTip(cutoffSlider, "Filter Cutoff Frequency");
    GuiHelpers::SetTip(resSlider,    "Filter Resonance");
    filterType.SetTooltip("Filter Type:\n2 Pole, 4 Pole, 8 Pole, Notch");
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
    // Slider-position-space approximation of the DSP's Hz-space modulation —
    // good enough visually, much cheaper.
    const float baseCutoff = (float) cutoffSlider.getValue();
    const float baseRes    = (float) resSlider   .getValue();

    const float envVal     = resources.filtEnvDisplay != nullptr ? resources.filtEnvDisplay->load() : 0.0f;
    const float lfoVal     = resources.filtLFODisplay != nullptr ? resources.filtLFODisplay->load() : 0.0f;

    const float coAmt      = envCOAmtPtr  != nullptr ? envCOAmtPtr ->load() : 0.0f;
    const float resAmt     = envResAmtPtr != nullptr ? envResAmtPtr->load() : 0.0f;
    const float lfoAmt     = lfoAmtPtr    != nullptr ? lfoAmtPtr   ->load() : 0.0f;

    const float cutoffHeadroomUp   = (1.0f - baseCutoff) * coAmt;
    const float cutoffHeadroomDown = baseCutoff          * lfoAmt;   // -ve LFO pulls below baseline
    const float cutoffHeadroomLfoU = (1.0f - baseCutoff) * lfoAmt;

    const float envCutoffOffset = envVal * cutoffHeadroomUp;
    const float lfoCutoffOffset = lfoVal >= 0.0f
                                    ? lfoVal * cutoffHeadroomLfoU
                                    : lfoVal * cutoffHeadroomDown;

    const float effCutoff = juce::jlimit(0.0f, 1.0f, baseCutoff + envCutoffOffset + lfoCutoffOffset);

    const float resHeadroom = (2.0f - baseRes) * resAmt;
    const float effRes      = juce::jlimit(1.0f, 2.0f, baseRes + envVal * resHeadroom);

    filterVisual.drawFilterShape(filterType.SelectedIndex.get() + 1
                                 , effCutoff
                                 , effRes);
}
