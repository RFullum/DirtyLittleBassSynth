/*
  ==============================================================================

    ModifierPanel.cpp

  ==============================================================================
*/

#include "ModifierPanel.h"
#include "GuiHelpers.h"

//============================================================

ModifierPanel::ModifierPanel(GuiResources &res)
: resources(res)
{
    auto primary = res.theme.primaryAccent;
    auto pink    = res.theme.pinkAccent;
    auto orange  = res.theme.orangeAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    DLBS::SetupSlider(this, portaSlider,         juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, primary, thumb, txt);
    DLBS::SetupSlider(this, foldbackSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    DLBS::SetupSlider(this, ringToneSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,   thumb, txt);
    DLBS::SetupSlider(this, ringPitchSlider,     juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,   thumb, txt);
    DLBS::SetupSlider(this, ringDryWetSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,   thumb, txt);
    DLBS::SetupSlider(this, frqShftPitchSlider,  juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);
    DLBS::SetupSlider(this, frqShftDryWetSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);
    DLBS::SetupSlider(this, sHPitchSlider,       juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);
    DLBS::SetupSlider(this, sHDryWetSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);

    portaSlider        .setLookAndFeel(res.dialLookAndFeel);
    foldbackSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringToneSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringPitchSlider    .setLookAndFeel(res.dialLookAndFeel);
    ringDryWetSlider   .setLookAndFeel(res.dryWetLookAndFeel);
    frqShftPitchSlider .setLookAndFeel(res.dialLookAndFeel);
    frqShftDryWetSlider.setLookAndFeel(res.dryWetLookAndFeel);
    sHPitchSlider      .setLookAndFeel(res.dialLookAndFeel);
    sHDryWetSlider     .setLookAndFeel(res.dryWetLookAndFeel);
    
    portaSlider        .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    foldbackSlider     .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    ringToneSlider     .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    ringPitchSlider    .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    ringDryWetSlider   .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    frqShftPitchSlider .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    frqShftDryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sHPitchSlider      .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sHDryWetSlider     .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    DLBS::SetupSectionLabel(this, sectionLabel, "Modifiers", res.theme.textSecondary);

    DLBS::SetupLabel(this, portaLabel,    "Portamento",           txt, 14.0f);
    DLBS::SetupLabel(this, foldbackLabel, "Foldback\nDistortion", txt, 14.0f);
    DLBS::SetupLabel(this, ringLabel,    "Ring Mod",      txt, 14.0f);
    DLBS::SetupLabel(this, frqShftLabel, "Freq Shift",    txt, 14.0f);
    DLBS::SetupLabel(this, sHLabel,      "Sample & Hold", txt, 14.0f);
    DLBS::SetupLabel(this, toneLabel,    "Tone",          txt, 12.0f);
    DLBS::SetupLabel(this, pitchLabel,   "Pitch",         txt, 12.0f);
    DLBS::SetupLabel(this, dryWetLabel,  "Dry/Wet",       txt, 12.0f);

    portaAtt         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "porta_time",       portaSlider);
    foldbackAtt      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "foldback_dist",    foldbackSlider);
    ringToneAtt      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "ring_tone",        ringToneSlider);
    ringPitchAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "ring_mod_pitch",   ringPitchSlider);
    ringDryWetAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "ring_mod_mix",     ringDryWetSlider);
    frqShftPitchAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "freq_shift_pitch", frqShftPitchSlider);
    frqShftDryWetAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "freq_shift_mix",   frqShftDryWetSlider);
    sHPitchAtt       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "sandh_pitch",      sHPitchSlider);
    sHDryWetAtt      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "sandh_mix",        sHDryWetSlider);
}

void ModifierPanel::resized()
{
    static constexpr int sectionSpacerSize = 2;
    static constexpr int rowLabelH         = 35;
    static constexpr int topSliderSize     = 65;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);
    sectionLabel.setBounds(bounds.removeFromTop(16));
    bounds.removeFromTop(50);
    
    auto      topArea   = bounds .removeFromTop(125);
    auto      portaArea = topArea.removeFromLeft(topArea.proportionOfWidth(0.5f));
    portaLabel    .setBounds(portaArea.removeFromTop(rowLabelH));
    portaSlider   .setBounds(portaArea.withSizeKeepingCentre(topSliderSize, topSliderSize));
    foldbackLabel .setBounds(topArea.removeFromTop(rowLabelH));
    foldbackSlider.setBounds(topArea.withSizeKeepingCentre(topSliderSize, topSliderSize));
    
    bounds.removeFromTop(25);
    bounds.removeFromRight(10);
    const int modSectionGridWidth = bounds.getWidth() / 4;
    auto      headingsSpace       = bounds.removeFromTop(20);
    headingsSpace.removeFromLeft(modSectionGridWidth);    // skip the row-name column
    toneLabel    .setBounds(headingsSpace.removeFromLeft(modSectionGridWidth));
    pitchLabel   .setBounds(headingsSpace.removeFromLeft(modSectionGridWidth));
    dryWetLabel  .setBounds(headingsSpace.removeFromLeft(modSectionGridWidth));

    static constexpr int rowHeight = 65;

    auto ringModRow  = bounds.removeFromTop(rowHeight);
    auto freqShftRow = bounds.removeFromTop(rowHeight);
    auto sAndHRow    = bounds.removeFromTop(rowHeight);
    ringLabel       .setBounds(ringModRow.removeFromLeft(modSectionGridWidth));
    ringToneSlider  .setBounds(ringModRow.removeFromLeft(modSectionGridWidth));
    ringPitchSlider .setBounds(ringModRow.removeFromRight(modSectionGridWidth));
    ringDryWetSlider.setBounds(ringModRow.removeFromRight(modSectionGridWidth));
    
    frqShftLabel       .setBounds(freqShftRow.removeFromLeft(modSectionGridWidth));
    freqShftRow        .removeFromLeft(modSectionGridWidth);
    frqShftPitchSlider .setBounds(freqShftRow.removeFromLeft(modSectionGridWidth));
    frqShftDryWetSlider.setBounds(freqShftRow.removeFromLeft(modSectionGridWidth));
    
    sHLabel       .setBounds(sAndHRow.removeFromLeft(modSectionGridWidth));
    sAndHRow      .removeFromLeft(modSectionGridWidth);
    sHPitchSlider .setBounds(sAndHRow.removeFromLeft(modSectionGridWidth));
    sHDryWetSlider.setBounds(sAndHRow.removeFromLeft(modSectionGridWidth));
}
