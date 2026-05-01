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
    using namespace dlbs;

    auto primary = res.theme.primaryAccent;
    auto pink    = res.theme.pinkAccent;
    auto orange  = res.theme.orangeAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    SetupSlider(this, portaSlider,         juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, primary, thumb, txt);
    SetupSlider(this, foldbackSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    SetupSlider(this, ringToneSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,   thumb, txt);
    SetupSlider(this, ringPitchSlider,     juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,   thumb, txt);
    SetupSlider(this, ringDryWetSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,   thumb, txt);
    SetupSlider(this, frqShftPitchSlider,  juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);
    SetupSlider(this, frqShftDryWetSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);
    SetupSlider(this, sHPitchSlider,       juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);
    SetupSlider(this, sHDryWetSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange, thumb, txt);

    portaSlider        .setLookAndFeel(res.dialLookAndFeel);
    foldbackSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringToneSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringPitchSlider    .setLookAndFeel(res.dialLookAndFeel);
    ringDryWetSlider   .setLookAndFeel(res.dryWetLookAndFeel);
    frqShftPitchSlider .setLookAndFeel(res.dialLookAndFeel);
    frqShftDryWetSlider.setLookAndFeel(res.dryWetLookAndFeel);
    sHPitchSlider      .setLookAndFeel(res.dialLookAndFeel);
    sHDryWetSlider     .setLookAndFeel(res.dryWetLookAndFeel);

    SetupSectionLabel(this, sectionLabel, "Modifiers", res.theme.textSecondary);

    SetupLabel(this, portaLabel,    "Portamento",           txt, 14.0f);
    SetupLabel(this, foldbackLabel, "Foldback\nDistortion", txt, 14.0f);
    SetupLabel(this, ringLabel,    "Ring Mod",      txt, 14.0f);
    SetupLabel(this, frqShftLabel, "Freq Shift",    txt, 14.0f);
    SetupLabel(this, sHLabel,      "Sample & Hold", txt, 14.0f);
    SetupLabel(this, toneLabel,    "Tone",          txt, 16.0f);
    SetupLabel(this, pitchLabel,   "Pitch",         txt, 16.0f);
    SetupLabel(this, dryWetLabel,  "Dry/Wet",       txt, 16.0f);

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
    constexpr int sectionSpacerSize = 2;
    constexpr int modHeadingHeight  = 30;
    constexpr int driveRowHeight    = 90;
    constexpr int driveLabelHeight  = 26;

    auto area              = getLocalBounds().reduced(sectionSpacerSize);
    int  modSectionGridWidth = area.getWidth() / 4;

    sectionLabel.setBounds(area.removeFromTop(16).reduced(8, 0));

    // Top: Portamento + Foldback Distortion knobs side-by-side, label above each.
    auto driveRow        = area.removeFromTop(driveRowHeight);
    auto portaCell       = driveRow.removeFromLeft(driveRow.getWidth() / 2);
    auto portaLabelArea  = portaCell.removeFromTop(driveLabelHeight);
    auto foldLabelArea   = driveRow.removeFromTop(driveLabelHeight);

    portaLabel    .setBounds(portaLabelArea);
    portaSlider   .setBounds(portaCell);
    foldbackLabel .setBounds(foldLabelArea);
    foldbackSlider.setBounds(driveRow);

    // Top header row: shared column labels (Tone / Pitch / Dry-Wet).
    auto headingsSpace = area.removeFromTop(modHeadingHeight);
    headingsSpace.removeFromLeft(modSectionGridWidth);    // skip the row-name column
    auto toneHeading  = headingsSpace.removeFromLeft(modSectionGridWidth);
    auto pitchHeading = headingsSpace.removeFromLeft(modSectionGridWidth);

    toneLabel  .setBounds(toneHeading);
    pitchLabel .setBounds(pitchHeading);
    dryWetLabel.setBounds(headingsSpace);

    int rowHeight = area.getHeight() / 3;

    // Row-name labels (left column).
    auto modTypeColumn = area.removeFromLeft(modSectionGridWidth);
    auto ringNameArea  = modTypeColumn.removeFromTop(rowHeight);
    auto frqNameArea   = modTypeColumn.removeFromTop(rowHeight);

    ringLabel   .setBounds(ringNameArea);
    frqShftLabel.setBounds(frqNameArea);
    sHLabel     .setBounds(modTypeColumn);

    // Ring Mod row (Tone / Pitch / Dry-Wet).
    auto ringKnobs   = area.removeFromTop(rowHeight);
    auto ringTonePos = ringKnobs.removeFromLeft(modSectionGridWidth);
    auto ringPitchPos = ringKnobs.removeFromLeft(modSectionGridWidth);

    ringToneSlider  .setBounds(ringTonePos);
    ringPitchSlider .setBounds(ringPitchPos);
    ringDryWetSlider.setBounds(ringKnobs);

    // Freq Shift row (no Tone, just Pitch / Dry-Wet).
    auto frqKnobs = area.removeFromTop(rowHeight);
    frqKnobs.removeFromLeft(modSectionGridWidth);                       // skip Tone column
    auto frqPitchPos = frqKnobs.removeFromLeft(modSectionGridWidth);

    frqShftPitchSlider .setBounds(frqPitchPos);
    frqShftDryWetSlider.setBounds(frqKnobs);

    // Sample & Hold row (no Tone, just Pitch / Dry-Wet).
    area.removeFromLeft(modSectionGridWidth);                            // skip Tone column
    auto sHPitchPos = area.removeFromLeft(modSectionGridWidth);

    sHPitchSlider .setBounds(sHPitchPos);
    sHDryWetSlider.setBounds(area);
}
