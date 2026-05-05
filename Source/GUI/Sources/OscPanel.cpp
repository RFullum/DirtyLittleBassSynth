/*
  ==============================================================================

    OscPanel.cpp

  ==============================================================================
*/

#include "OscPanel.h"
#include "GuiHelpers.h"

//============================================================

OscPanel::OscPanel(GuiResources &res)
{
    auto accent = res.theme.primaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    DLBS::SetupSlider(this, oscMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    DLBS::SetupSlider(this, subMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    DLBS::SetupSlider(this, subGainSlider,        juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    DLBS::SetupSlider(this, pitchBendRangeSlider, juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    // Morph sliders sit directly under their wave visuals; suppress the value textbox
    // so the slider track spans the full width of the visual above it.
    oscMorphSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    subMorphSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    oscMorphSlider      .setLookAndFeel(res.dialLookAndFeel);
    subMorphSlider      .setLookAndFeel(res.dialLookAndFeel);
    subGainSlider       .setLookAndFeel(res.dialLookAndFeel);
    pitchBendRangeSlider.setLookAndFeel(res.dialLookAndFeel);

    DLBS::SetupSectionLabel(this, sectionLabel, "Oscillator", res.theme.textSecondary);

    static constexpr float fontSizeLarge = 24.0f;
    static constexpr float fontSizeSmall = 13.0f;
    DLBS::SetupLabel(this, oscLabel,            "OSC",         txt, fontSizeLarge, juce::Justification::centredRight);
    DLBS::SetupLabel(this, morphOscLabel,       "MORPH",       txt, fontSizeSmall, juce::Justification::centredRight);
    DLBS::SetupLabel(this, subLabel,            "SUB",         txt, fontSizeLarge, juce::Justification::centredRight);
    DLBS::SetupLabel(this, morphSubLabel,       "MORPH",       txt, fontSizeSmall, juce::Justification::centredRight);
    DLBS::SetupLabel(this, subGainLabel,        "Sub Gain",    txt, fontSizeSmall);
    DLBS::SetupLabel(this, pitchBendRangeLabel, "Bend",        txt, fontSizeSmall);

    subOctave.Setup(*res.apvts, "sub_osc_octave"
                    , juce::StringArray({"0", "-1", "-2"})
                    , accent
                    , res.theme.structure
                    , res.theme.textSecondary);
    addAndMakeVisible(subOctave);

    oscMorphAtt       = DLBS::AttachSlider(*res.apvts, "osc_morph",     oscMorphSlider);
    subMorphAtt       = DLBS::AttachSlider(*res.apvts, "sub_osc_morph", subMorphSlider);
    subGainAtt        = DLBS::AttachSlider(*res.apvts, "sub_osc_gain",  subGainSlider);

    // pitch_bend_range is intentionally NOT registered for MIDI Learn — it's a
    // setup parameter, not a live-tweak control. Keep the raw attachment so the
    // slider isn't tagged with a paramID.
    pitchBendRangeAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "pitch_bend_range", pitchBendRangeSlider);

    DLBS::SetSliderTextFormat(subGainSlider, DLBS::FormatGainDb);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();

    oscVisual   .SetColors(accent, bg, bgFade);
    subOscVisual.SetColors(accent, bg, bgFade);

    oscVisual   .Init(res.apvts->getRawParameterValue("osc_morph"),     /*useSquare*/ false);
    subOscVisual.Init(res.apvts->getRawParameterValue("sub_osc_morph"), /*useSquare*/ true,
                      res.apvts->getRawParameterValue("sub_osc_gain"));

    addAndMakeVisible(oscVisual);
    addAndMakeVisible(subOscVisual);
}

void OscPanel::resized()
{
    static constexpr float morphProp      = 0.2f;
    static constexpr float rightLabelProp = 0.075f;
    
    auto bounds = getLocalBounds();
    sectionLabel.setBounds(bounds.removeFromTop(16));
    
    const int margin = bounds.proportionOfWidth(0.15f);
    bounds.removeFromBottom(2);
    auto buttonsArea = bounds.removeFromBottom(bounds.proportionOfHeight(0.1f));
    auto subArea     = bounds.removeFromBottom(bounds.proportionOfHeight(0.5f));
    auto oscArea     = bounds;
    
    auto oscMargin = oscArea.removeFromLeft(margin);
    auto rangeArea = oscArea.removeFromRight(margin);
    morphOscLabel       .setBounds(oscMargin.removeFromBottom(oscMargin.proportionOfHeight(morphProp)));
    oscLabel            .setBounds(oscMargin);
    pitchBendRangeLabel .setBounds(rangeArea.removeFromTop(rangeArea.proportionOfHeight(rightLabelProp)));
    rangeArea           .removeFromBottom(rangeArea.proportionOfHeight(rightLabelProp));
    pitchBendRangeSlider.setBounds(rangeArea);
    oscMorphSlider      .setBounds(oscArea.removeFromBottom(oscArea.proportionOfHeight(morphProp)));
    oscVisual           .setBounds(oscArea);
    
    auto subMargin = subArea.removeFromLeft(margin);
    auto gainArea  = subArea.removeFromRight(margin);
    morphSubLabel .setBounds(subMargin.removeFromBottom(subMargin.proportionOfHeight(morphProp)));
    subLabel      .setBounds(subMargin);
    subGainLabel  .setBounds(gainArea.removeFromTop(gainArea.proportionOfHeight(rightLabelProp)));
    gainArea      .removeFromBottom(gainArea.proportionOfHeight(rightLabelProp));
    subGainSlider .setBounds(gainArea);
    subMorphSlider.setBounds(subArea.removeFromBottom(subArea.proportionOfHeight(morphProp)));
    subOscVisual  .setBounds(subArea);
    
    subOctave.setBounds(buttonsArea.reduced(15, 4));
}

void OscPanel::Update()
{
    oscVisual   .Update();
    subOscVisual.Update();
}
