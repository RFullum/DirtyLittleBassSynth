/*
  ==============================================================================

    FilterAdsrPanel.cpp

  ==============================================================================
*/

#include "FilterAdsrPanel.h"
#include "GuiHelpers.h"

//============================================================

FilterAdsrPanel::FilterAdsrPanel(GuiResources &res)
: resources(res)
{
    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    DLBS::SetupSlider(this, fltAttackSlider,    juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    DLBS::SetupSlider(this, fltDecaySlider,     juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    DLBS::SetupSlider(this, fltSustainSlider,   juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    DLBS::SetupSlider(this, fltReleaseSlider,   juce::Slider::SliderStyle::LinearHorizontal,             accent, thumb, txt);
    DLBS::SetupSlider(this, adsrToCutoffSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, accent, thumb, txt);
    DLBS::SetupSlider(this, adsrToResSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, accent, thumb, txt);

    fltAttackSlider   .setLookAndFeel(res.dialLookAndFeel);
    fltDecaySlider    .setLookAndFeel(res.dialLookAndFeel);
    fltSustainSlider  .setLookAndFeel(res.dialLookAndFeel);
    fltReleaseSlider  .setLookAndFeel(res.dialLookAndFeel);
    adsrToCutoffSlider.setLookAndFeel(res.dryWetLookAndFeel);
    adsrToResSlider   .setLookAndFeel(res.dryWetLookAndFeel);

    DLBS::SetupSectionLabel(this, sectionLabel, "Filter Env", res.theme.textSecondary);

    DLBS::SetupLabel(this, fltAttackLabel,    "A",         txt, 16.0f);
    DLBS::SetupLabel(this, fltDecayLabel,     "D",         txt, 16.0f);
    DLBS::SetupLabel(this, fltSustainLabel,   "S",         txt, 16.0f);
    DLBS::SetupLabel(this, fltReleaseLabel,   "R",         txt, 16.0f);
    DLBS::SetupLabel(this, adsrToCutoffLabel, "To Cutoff", txt, 15.0f);
    DLBS::SetupLabel(this, adsrToResLabel,    "To Rez",    txt, 15.0f);

    attackAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_attack",  fltAttackSlider);
    decayAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_decay",   fltDecaySlider);
    sustainAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_sustain", fltSustainSlider);
    releaseAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_release", fltReleaseSlider);
    toCutoffAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_COAmt",   adsrToCutoffSlider);
    toResAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_ResAmt",  adsrToResSlider);

    DLBS::SetSliderTextFormat(fltAttackSlider,  DLBS::FormatTime);
    DLBS::SetSliderTextFormat(fltDecaySlider,   DLBS::FormatTime);
    DLBS::SetSliderTextFormat(fltSustainSlider, DLBS::FormatPercent);
    DLBS::SetSliderTextFormat(fltReleaseSlider, DLBS::FormatTime);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    adsrVisual.SetColors(accent, bg, bgFade);
    adsrVisual.Init(res.apvts->getRawParameterValue("filtEnv_attack"),
                    res.apvts->getRawParameterValue("filtEnv_decay"),
                    res.apvts->getRawParameterValue("filtEnv_sustain"),
                    res.apvts->getRawParameterValue("filtEnv_release"));

    addAndMakeVisible(adsrVisual);
}

void FilterAdsrPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int filtLabelHeight   = 30;
    constexpr int adsrLabelWidth    = 18;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(bounds.removeFromTop(16).reduced(8, 0));

    auto reduced = bounds;

    // Right third: To Cutoff / To Res rotary knobs (unchanged).
    auto rotaryArea     = reduced.removeFromRight(reduced.getWidth() / 3);
    auto toResArea      = rotaryArea.removeFromBottom(rotaryArea.getHeight() / 2);
    auto toResLabelArea = toResArea.removeFromTop(filtLabelHeight - 9);
    auto toCOLabelArea  = rotaryArea.removeFromTop(filtLabelHeight - 9);

    adsrToResLabel    .setBounds(toResLabelArea);
    adsrToResSlider   .setBounds(toResArea);
    adsrToCutoffLabel .setBounds(toCOLabelArea);
    adsrToCutoffSlider.setBounds(rotaryArea);

    // Left two-thirds: envelope visual on top, then 4 stacked horizontal ADSR
    // rows, each with letter label on the left and the slider filling the rest.
    auto slidersArea = reduced.reduced(sectionSpacerSize);

    const int visualHeight = slidersArea.getHeight() / 3;
    auto      visualArea   = slidersArea.removeFromTop(visualHeight);
    adsrVisual.setBounds(visualArea);

    const int rowHeight = slidersArea.getHeight() / 4;

    auto layoutRow = [&](juce::Slider &slider, juce::Label &label)
    {
        auto row     = slidersArea.removeFromTop(rowHeight);
        auto labelBx = row.removeFromLeft(adsrLabelWidth);

        label .setBounds(labelBx);
        slider.setBounds(row);
    };

    layoutRow(fltAttackSlider,  fltAttackLabel);
    layoutRow(fltDecaySlider,   fltDecayLabel);
    layoutRow(fltSustainSlider, fltSustainLabel);
    layoutRow(fltReleaseSlider, fltReleaseLabel);
}

void FilterAdsrPanel::Update()
{
    adsrVisual.Update();
}
