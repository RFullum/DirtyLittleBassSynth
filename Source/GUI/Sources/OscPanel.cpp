/*
  ==============================================================================

    OscPanel.cpp

  ==============================================================================
*/

#include "OscPanel.h"

#include "GuiHelpers.h"
#include "GUI/GuiHelpers.h"
#include "GUI/Format.h"
#include "GUI/MidiLearnAttachments.h"

//==============================================================================

OscPanel::OscPanel(GuiResources &res)
: subOctave(*res.apvts
            , "sub_osc_octave"
            , res.theme
            , juce::StringArray({"0","-1","-2"})
            , juce::FontOptions("Helvetica", 12.0f, juce::Font::bold))
{
    setOpaque(false);
    auto accent = res.theme.primaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;
    
    GuiHelpers::SetupSlider(this, oscMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    GuiHelpers::SetupSlider(this, subMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    GuiHelpers::SetupSlider(this, subGainSlider,        juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    GuiHelpers::SetupSlider(this, pitchBendRangeSlider, juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    
    // No textbox: track spans the full width of the visual above.
    oscMorphSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    subMorphSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    oscMorphSlider      .setLookAndFeel(res.dialLookAndFeel);
    subMorphSlider      .setLookAndFeel(res.dialLookAndFeel);
    subGainSlider       .setLookAndFeel(res.dialLookAndFeel);
    pitchBendRangeSlider.setLookAndFeel(res.dialLookAndFeel);
    
    GuiHelpers::SetupSectionLabel(this, sectionLabel, "Oscillator", res.theme.textSecondary);
    
    static constexpr float fontSizeLarge = 24.0f;
    static constexpr float fontSizeSmall = 13.0f;
    const juce::Font::FontStyleFlags style = juce::Font::bold;
    GuiHelpers::SetupLabel(this, oscLabel,            "OSC",         txt, fontSizeLarge, style, juce::Justification::centredRight);
    GuiHelpers::SetupLabel(this, morphOscLabel,       "MORPH",       txt, fontSizeSmall, style, juce::Justification::centredRight);
    GuiHelpers::SetupLabel(this, subLabel,            "SUB",         txt, fontSizeLarge, style, juce::Justification::centredRight);
    GuiHelpers::SetupLabel(this, morphSubLabel,       "MORPH",       txt, fontSizeSmall, style, juce::Justification::centredRight);
    GuiHelpers::SetupLabel(this, subGainLabel,        "Sub Gain",    txt, fontSizeSmall);
    GuiHelpers::SetupLabel(this, pitchBendRangeLabel, "Bend",        txt, fontSizeSmall);
    
    subOctave.getProperties().set(MidiLearn::ParamIDProperty, "sub_osc_octave");
    addAndMakeVisible(subOctave);
    
    oscMorphAtt       = MidiLearn::AttachSlider(*res.apvts, "osc_morph",     oscMorphSlider);
    subMorphAtt       = MidiLearn::AttachSlider(*res.apvts, "sub_osc_morph", subMorphSlider);
    subGainAtt        = MidiLearn::AttachSlider(*res.apvts, "sub_osc_gain",  subGainSlider);
    
    // Raw attachment (not MidiLearn::AttachSlider): keeps pitch_bend_range out of
    // MIDI Learn — setup param, not a live-tweak control.
    pitchBendRangeAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "pitch_bend_range", pitchBendRangeSlider);
    
    Format::SetSliderTextFormat(subGainSlider, Format::GainDb);
    
    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    
    oscVisual   .SetColors(accent, bg, bgFade);
    subOscVisual.SetColors(accent, bg, bgFade);
    
    oscVisual   .Init(res.apvts->getRawParameterValue("osc_morph"),     /*useSquare*/ false);
    subOscVisual.Init(res.apvts->getRawParameterValue("sub_osc_morph"), /*useSquare*/ true,
                      res.apvts->getRawParameterValue("sub_osc_gain"));
    
    addAndMakeVisible(oscVisual);
    addAndMakeVisible(subOscVisual);
    
    GuiHelpers::SetTip(oscMorphSlider,       "Morph Main oscillator: Sine - Spike - Saw");
    GuiHelpers::SetTip(subMorphSlider,       "Morph Sub oscillator: Sine - Square - Saw");
    GuiHelpers::SetTip(subGainSlider,        "Sub oscillator gain\n0.0dB is unity with Main Oscillator");
    GuiHelpers::SetTip(pitchBendRangeSlider, "Pitch Bend Range");
    subOctave.SetTooltip("Sub Osc number of octaves below Main Osc");
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
