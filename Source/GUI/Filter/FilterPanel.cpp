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
    setOpaque(false);

    envCOAmtPtr  = res.apvts->getRawParameterValue("filtEnv_COAmt");
    envResAmtPtr = res.apvts->getRawParameterValue("filtEnv_ResAmt");
    lfoAmtPtr    = res.apvts->getRawParameterValue("filtLFO_amt");
    
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

    cutoffAtt = DLBS::AttachSlider(*res.apvts, "filter_cutoff", cutoffSlider);
    resAtt    = DLBS::AttachSlider(*res.apvts, "filter_res",    resSlider);

    DLBS::SetSliderTextFormat(cutoffSlider, DLBS::FormatCutoffHz);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    filterVisual.setColors(accent, bg, bgFade);

    addAndMakeVisible(filterVisual);

    DLBS::SetTip(cutoffSlider, "Filter Cutoff Frequency");
    DLBS::SetTip(resSlider,    "Filter Resonance");
    DLBS::SetTip(filterType,   "Filter Type:\n2 Pole, 4 Pole, 8 Pole, Notch");
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
    // Compute the effective cutoff / resonance in slider-position space (0..1)
    // by stacking the env and LFO modulations on top of the base slider values.
    // This is a deliberately simpler model than the DSP (which works in Hz with
    // exponential mapping and asymmetric LFO headroom) — the visual already
    // operates in slider space, and the animation only needs to read as "the
    // filter is being pushed around," not be sample-accurate.
    const float baseCutoff = (float) cutoffSlider.getValue();
    const float baseRes    = (float) resSlider   .getValue();

    const float envVal     = resources.filtEnvDisplay != nullptr ? resources.filtEnvDisplay->load() : 0.0f;
    const float lfoVal     = resources.filtLFODisplay != nullptr ? resources.filtLFODisplay->load() : 0.0f;

    const float coAmt      = envCOAmtPtr  != nullptr ? envCOAmtPtr ->load() : 0.0f;
    const float resAmt     = envResAmtPtr != nullptr ? envResAmtPtr->load() : 0.0f;
    const float lfoAmt     = lfoAmtPtr    != nullptr ? lfoAmtPtr   ->load() : 0.0f;

    const float effCutoff  = juce::jlimit(0.0f, 1.0f, baseCutoff + envVal * coAmt + lfoVal * lfoAmt);
    const float effRes     = juce::jlimit(0.0f, 1.0f, baseRes    + envVal * resAmt);

    filterVisual.drawFilterShape(filterType.GetSelectedIndex() + 1
                                 , effCutoff
                                 , effRes);
}
