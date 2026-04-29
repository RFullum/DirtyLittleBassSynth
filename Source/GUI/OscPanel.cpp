/*
  ==============================================================================

    OscPanel.cpp

  ==============================================================================
*/

#include "OscPanel.h"
#include "GuiHelpers.h"

//============================================================

OscPanel::OscPanel(GuiResources &res)
: resources(res)
{
    using namespace dlbs;

    auto accent  = res.theme.primaryAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    SetupSlider(this, oscMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    SetupSlider(this, subMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    SetupSlider(this, subGainSlider,        juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    SetupSlider(this, pitchBendRangeSlider, juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    // Morph sliders sit directly under their wave visuals; suppress the value textbox
    // so the slider track spans the full width of the visual above it.
    oscMorphSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    subMorphSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    oscMorphSlider      .setLookAndFeel(res.dialLookAndFeel);
    subMorphSlider      .setLookAndFeel(res.dialLookAndFeel);
    subGainSlider       .setLookAndFeel(res.dialLookAndFeel);
    pitchBendRangeSlider.setLookAndFeel(res.dialLookAndFeel);

    SetupSectionLabel(this, sectionLabel, "Oscillator", res.theme.textSecondary);

    SetupLabel(this, oscMorphLabel,       "OSC",         txt, 18.0f);
    SetupLabel(this, oscMorphLabel2,      "MORPH",       txt, 15.0f);
    SetupLabel(this, subMorphLabel,       "SUB",         txt, 18.0f);
    SetupLabel(this, subMorphLabel2,      "MORPH",       txt, 15.0f);
    SetupLabel(this, subGainLabel,        "Sub Gain",    txt, 13.0f);
    SetupLabel(this, pitchBendRangeLabel, "Bend\nRange", txt, 13.0f);

    SetupComboBox(this, subOctave, juce::StringArray({"0", "-1 Oct", "-2 Oct"}));

    oscMorphAtt       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "osc_morph",        oscMorphSlider);
    subMorphAtt       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "sub_osc_morph",    subMorphSlider);
    subGainAtt        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "sub_osc_gain",     subGainSlider);
    pitchBendRangeAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*res.apvts, "pitch_bend_range", pitchBendRangeSlider);
    subOctaveAtt      = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(*res.apvts, "sub_osc_octave",   subOctave);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();

    oscVisual   .SetColors(accent, bg, bgFade);
    subOscVisual.SetColors(accent, bg, bgFade);

    oscVisual   .Init(res.apvts->getRawParameterValue("osc_morph"),     /*useSquare*/ false);
    subOscVisual.Init(res.apvts->getRawParameterValue("sub_osc_morph"), /*useSquare*/ true);

    addAndMakeVisible(oscVisual);
    addAndMakeVisible(subOscVisual);
}

void OscPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int oscGainWidth      = 75;
    constexpr int subOctaveHeight   = 30;
    constexpr int morphLabelWidth   = 50;
    constexpr int gainLabelHeight   = 30;

    auto oscArea = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(oscArea.removeFromTop(16).reduced(8, 0));

    // Main osc (top half)
    auto mainOscArea        = oscArea.removeFromTop(oscArea.getHeight() / 2);
    auto mainOscAreaReduced = mainOscArea.reduced(sectionSpacerSize * 2);

    auto oscGainSpace         = mainOscAreaReduced.removeFromRight (oscGainWidth);
    auto morphLabelSpace      = mainOscAreaReduced.removeFromLeft  (morphLabelWidth);
    auto morphLabelSpace2     = morphLabelSpace.removeFromTop      (morphLabelSpace.getHeight() / 2);
    auto morphOSCLabelSpace   = morphLabelSpace2.removeFromBottom  ((int)(gainLabelHeight - 10));
    auto morphMORPHLabelSpace = morphLabelSpace.removeFromTop      ((int)(gainLabelHeight - 10));
    auto morphSliderSpace     = mainOscAreaReduced.removeFromBottom(gainLabelHeight);
    auto pitchBendLabelSpace  = oscGainSpace.removeFromTop         (gainLabelHeight);

    oscMorphLabel       .setBounds(morphOSCLabelSpace);
    oscMorphLabel2      .setBounds(morphMORPHLabelSpace);
    oscMorphSlider      .setBounds(morphSliderSpace);
    oscVisual           .setBounds(mainOscAreaReduced.reduced(sectionSpacerSize));
    pitchBendRangeSlider.setBounds(oscGainSpace);
    pitchBendRangeLabel .setBounds(pitchBendLabelSpace);

    // Sub osc (bottom half)
    auto subOscAreaReduced = oscArea.reduced(sectionSpacerSize * 2);

    auto subGainSpace            = subOscAreaReduced.removeFromRight  (oscGainWidth);
    auto subOctaveSpace          = subGainSpace.removeFromBottom      (subOctaveHeight);
    auto subGainLabelSpace       = subGainSpace.removeFromTop         (gainLabelHeight);
    auto subMorphLabelSpace      = subOscAreaReduced.removeFromLeft   (morphLabelWidth);
    auto subMorphLabelSpace2     = subMorphLabelSpace.removeFromTop   (subMorphLabelSpace.getHeight() / 2);
    auto subMorphOSCLabelSpace   = subMorphLabelSpace2.removeFromBottom((int)(gainLabelHeight - 10));
    auto subMorphMORPHLabelSpace = subMorphLabelSpace.removeFromTop   ((int)(gainLabelHeight - 10));
    auto subMorphSliderSpace     = subOscAreaReduced.removeFromBottom (gainLabelHeight);

    subGainLabel  .setBounds(subGainLabelSpace);
    subMorphLabel .setBounds(subMorphOSCLabelSpace);
    subMorphLabel2.setBounds(subMorphMORPHLabelSpace);
    subOctave     .setBounds(subOctaveSpace);
    subGainSlider .setBounds(subGainSpace);
    subMorphSlider.setBounds(subMorphSliderSpace);
    subOscVisual  .setBounds(subOscAreaReduced.reduced(sectionSpacerSize));
}

void OscPanel::Update()
{
    oscVisual   .Update();
    subOscVisual.Update();
}
