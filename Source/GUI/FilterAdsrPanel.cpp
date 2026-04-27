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
    using namespace dlbs;

    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    SetupSlider(this, fltAttackSlider,    juce::Slider::SliderStyle::LinearVertical,               accent, thumb, txt, true);
    SetupSlider(this, fltDecaySlider,     juce::Slider::SliderStyle::LinearVertical,               accent, thumb, txt, true);
    SetupSlider(this, fltSustainSlider,   juce::Slider::SliderStyle::LinearVertical,               accent, thumb, txt, true);
    SetupSlider(this, fltReleaseSlider,   juce::Slider::SliderStyle::LinearVertical,               accent, thumb, txt, true);
    SetupSlider(this, adsrToCutoffSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, accent, thumb, txt, false);
    SetupSlider(this, adsrToResSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, accent, thumb, txt, false);

    fltAttackSlider   .setLookAndFeel(res.dialLookAndFeel);
    fltDecaySlider    .setLookAndFeel(res.dialLookAndFeel);
    fltSustainSlider  .setLookAndFeel(res.dialLookAndFeel);
    fltReleaseSlider  .setLookAndFeel(res.dialLookAndFeel);
    adsrToCutoffSlider.setLookAndFeel(res.dryWetLookAndFeel);
    adsrToResSlider   .setLookAndFeel(res.dryWetLookAndFeel);

    SetupLabel(this, fltAttackLabel,    "A",         txt, 17.0f);
    SetupLabel(this, fltDecayLabel,     "D",         txt, 17.0f);
    SetupLabel(this, fltSustainLabel,   "S",         txt, 17.0f);
    SetupLabel(this, fltReleaseLabel,   "R",         txt, 17.0f);
    SetupLabel(this, adsrToCutoffLabel, "To Cutoff", txt, 15.0f);
    SetupLabel(this, adsrToResLabel,    "To Rez",    txt, 15.0f);

    attackAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_attack",  fltAttackSlider);
    decayAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_decay",   fltDecaySlider);
    sustainAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_sustain", fltSustainSlider);
    releaseAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_release", fltReleaseSlider);
    toCutoffAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_COAmt",   adsrToCutoffSlider);
    toResAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtEnv_ResAmt",  adsrToResSlider);
}

void FilterAdsrPanel::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRound);

    g.setColour(resources.theme.background);
    g.fillRoundedRectangle(innerBg, cornerRound);
}

void FilterAdsrPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int filtLabelHeight   = 30;

    auto area    = getLocalBounds().reduced(sectionSpacerSize);
    auto reduced = area;
    innerBg = reduced.toFloat();

    auto rotaryArea     = reduced.removeFromRight(reduced.getWidth() / 3);
    auto toResArea      = rotaryArea.removeFromBottom(rotaryArea.getHeight() / 2);
    auto toResLabelArea = toResArea.removeFromTop(filtLabelHeight - 9);
    auto toCOLabelArea  = rotaryArea.removeFromTop(filtLabelHeight - 9);

    adsrToResLabel    .setBounds(toResLabelArea);
    adsrToResSlider   .setBounds(toResArea);
    adsrToCutoffLabel .setBounds(toCOLabelArea);
    adsrToCutoffSlider.setBounds(rotaryArea);

    int sliderWidth = reduced.getWidth() / 4;
    auto headerRow  = reduced.removeFromTop(filtLabelHeight);
    auto aLabelArea = headerRow.removeFromLeft(sliderWidth);
    auto dLabelArea = headerRow.removeFromLeft(sliderWidth);
    auto sLabelArea = headerRow.removeFromLeft(sliderWidth);

    fltAttackLabel .setBounds(aLabelArea);
    fltDecayLabel  .setBounds(dLabelArea);
    fltSustainLabel.setBounds(sLabelArea);
    fltReleaseLabel.setBounds(headerRow);

    auto aSliderArea = reduced.removeFromLeft(sliderWidth);
    auto dSliderArea = reduced.removeFromLeft(sliderWidth);
    auto sSliderArea = reduced.removeFromLeft(sliderWidth);

    fltAttackSlider .setBounds(aSliderArea);
    fltDecaySlider  .setBounds(dSliderArea);
    fltSustainSlider.setBounds(sSliderArea);
    fltReleaseSlider.setBounds(reduced);
}
