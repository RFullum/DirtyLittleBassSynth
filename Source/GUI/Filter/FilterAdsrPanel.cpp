/*
  ==============================================================================

    FilterAdsrPanel.cpp

  ==============================================================================
*/

#include "FilterAdsrPanel.h"
#include "GuiHelpers.h"
#include "GUI/GuiHelpers.h"
#include "GUI/Format.h"

//==============================================================================

FilterAdsrPanel::FilterAdsrPanel(GuiResources &res)
{
    setOpaque(false);
    
    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    GuiHelpers::SetupSlider(this, fltAttackSlider,    juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    GuiHelpers::SetupSlider(this, fltDecaySlider,     juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    GuiHelpers::SetupSlider(this, fltSustainSlider,   juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    GuiHelpers::SetupSlider(this, fltReleaseSlider,   juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    GuiHelpers::SetupSlider(this, adsrToCutoffSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, accent, thumb, txt);
    GuiHelpers::SetupSlider(this, adsrToResSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, accent, thumb, txt);

    fltAttackSlider   .setLookAndFeel(res.dialLookAndFeel);
    fltDecaySlider    .setLookAndFeel(res.dialLookAndFeel);
    fltSustainSlider  .setLookAndFeel(res.dialLookAndFeel);
    fltReleaseSlider  .setLookAndFeel(res.dialLookAndFeel);
    adsrToCutoffSlider.setLookAndFeel(res.dryWetLookAndFeel);
    adsrToResSlider   .setLookAndFeel(res.dryWetLookAndFeel);
    
    // TODO: To CO and To Rez - remove slider value label?

    GuiHelpers::SetupSectionLabel(this, sectionLabel, "Filter Env", res.theme.textSecondary);

    GuiHelpers::SetupLabel(this, fltAttackLabel,    "A",         txt, 14.0f);
    GuiHelpers::SetupLabel(this, fltDecayLabel,     "D",         txt, 14.0f);
    GuiHelpers::SetupLabel(this, fltSustainLabel,   "S",         txt, 14.0f);
    GuiHelpers::SetupLabel(this, fltReleaseLabel,   "R",         txt, 14.0f);
    GuiHelpers::SetupLabel(this, adsrToCutoffLabel, "To Cutoff", txt, 14.0f);
    GuiHelpers::SetupLabel(this, adsrToResLabel,    "To Rez",    txt, 14.0f);

    attackAtt   = DLBS::AttachSlider(*res.apvts, "filtEnv_attack",  fltAttackSlider);
    decayAtt    = DLBS::AttachSlider(*res.apvts, "filtEnv_decay",   fltDecaySlider);
    sustainAtt  = DLBS::AttachSlider(*res.apvts, "filtEnv_sustain", fltSustainSlider);
    releaseAtt  = DLBS::AttachSlider(*res.apvts, "filtEnv_release", fltReleaseSlider);
    toCutoffAtt = DLBS::AttachSlider(*res.apvts, "filtEnv_COAmt",   adsrToCutoffSlider);
    toResAtt    = DLBS::AttachSlider(*res.apvts, "filtEnv_ResAmt",  adsrToResSlider);

    Format::SetSliderTextFormat(fltAttackSlider,  Format::TimeSec);
    Format::SetSliderTextFormat(fltDecaySlider,   Format::TimeSec);
    Format::SetSliderTextFormat(fltSustainSlider, Format::Percent);
    Format::SetSliderTextFormat(fltReleaseSlider, Format::TimeSec);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    adsrVisual.SetColors(accent, bg, bgFade);
    adsrVisual.Init(res.apvts->getRawParameterValue("filtEnv_attack")
                    , res.apvts->getRawParameterValue("filtEnv_decay")
                    , res.apvts->getRawParameterValue("filtEnv_sustain")
                    , res.apvts->getRawParameterValue("filtEnv_release"));

    addAndMakeVisible(adsrVisual);

    GuiHelpers::SetTip(fltAttackSlider,    "Filter Attack");
    GuiHelpers::SetTip(fltDecaySlider,     "Filter Decay");
    GuiHelpers::SetTip(fltSustainSlider,   "Filter Sustain");
    GuiHelpers::SetTip(fltReleaseSlider,   "Filter Release");
    GuiHelpers::SetTip(adsrToCutoffSlider, "Envelop to Cutoff Frequency Amount");
    GuiHelpers::SetTip(adsrToResSlider,    "Envelop to Resonance Frequency Amount"); 
}

void FilterAdsrPanel::resized()
{
    auto bounds = getLocalBounds().reduced(2);
    sectionLabel.setBounds(bounds.removeFromTop(16));
    
    auto      amountArea    = bounds.removeFromRight(70);
    const int amountSliceH  = amountArea.getHeight() / 9;
    const int amountSliderH = amountSliceH * 3;
    adsrToCutoffLabel .setBounds(amountArea.removeFromTop(amountSliceH));
    adsrToCutoffSlider.setBounds(amountArea.removeFromTop(amountSliderH).withSizeKeepingCentre(70, amountSliderH));
    
    amountArea     .removeFromTop(amountSliceH);
    adsrToResLabel .setBounds(amountArea.removeFromTop(amountSliceH));
    adsrToResSlider.setBounds(amountArea.removeFromTop(amountSliderH)
                                        .withSizeKeepingCentre(50, amountSliderH));
    
    auto      sliderArea = bounds.removeFromBottom(bounds.proportionOfHeight(0.5f));
    const int rowH       = sliderArea.getHeight() / 4;
    
    auto adsrRow = [&](juce::Slider &slider, juce::Label &label)
    {
        auto row = sliderArea.removeFromTop(rowH);
        label .setBounds(row.removeFromLeft(19));
        slider.setBounds(row);
    };
    adsrRow(fltAttackSlider,  fltAttackLabel);
    adsrRow(fltDecaySlider,   fltDecayLabel);
    adsrRow(fltSustainSlider, fltSustainLabel);
    adsrRow(fltReleaseSlider, fltReleaseLabel);
    
    bounds.removeFromLeft(24);
    bounds.removeFromRight(55);
    adsrVisual.setBounds(bounds);
}

void FilterAdsrPanel::Update()
{
    adsrVisual.Update();
}
