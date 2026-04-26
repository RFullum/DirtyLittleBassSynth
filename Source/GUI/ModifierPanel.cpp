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

    SetupSlider(this, ringToneSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, res.fieryRose, res.fieryRose, res.textColor, false);
    SetupSlider(this, ringPitchSlider,     juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, res.fieryRose, res.fieryRose, res.textColor, false);
    SetupSlider(this, ringDryWetSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, res.fieryRose, res.fieryRose, res.textColor, false);
    SetupSlider(this, frqShftPitchSlider,  juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, res.fieryRose, res.fieryRose, res.textColor, false);
    SetupSlider(this, frqShftDryWetSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, res.fieryRose, res.fieryRose, res.textColor, false);
    SetupSlider(this, sHPitchSlider,       juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, res.fieryRose, res.fieryRose, res.textColor, false);
    SetupSlider(this, sHDryWetSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, res.fieryRose, res.fieryRose, res.textColor, false);

    ringToneSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringPitchSlider    .setLookAndFeel(res.dialLookAndFeel);
    ringDryWetSlider   .setLookAndFeel(res.dryWetLookAndFeel);
    frqShftPitchSlider .setLookAndFeel(res.dialLookAndFeel);
    frqShftDryWetSlider.setLookAndFeel(res.dryWetLookAndFeel);
    sHPitchSlider      .setLookAndFeel(res.dialLookAndFeel);
    sHDryWetSlider     .setLookAndFeel(res.dryWetLookAndFeel);

    SetupLabel(this, ringLabel,    "Ring Mod",      res.textColor, 14.0f);
    SetupLabel(this, frqShftLabel, "Freq Shift",    res.textColor, 14.0f);
    SetupLabel(this, sHLabel,      "Sample & Hold", res.textColor, 14.0f);
    SetupLabel(this, toneLabel,    "Tone",          res.onyx,      16.0f);
    SetupLabel(this, pitchLabel,   "Pitch",         res.onyx,      16.0f);
    SetupLabel(this, dryWetLabel,  "Dry/Wet",       res.onyx,      16.0f);

    ringToneAtt      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "ring_tone",        ringToneSlider);
    ringPitchAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "ring_mod_pitch",   ringPitchSlider);
    ringDryWetAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "ring_mod_mix",     ringDryWetSlider);
    frqShftPitchAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "freq_shift_pitch", frqShftPitchSlider);
    frqShftDryWetAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "freq_shift_mix",   frqShftDryWetSlider);
    sHPitchAtt       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "sandh_pitch",      sHPitchSlider);
    sHDryWetAtt      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "sandh_mix",        sHDryWetSlider);
}

void ModifierPanel::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    g.setColour(resources.magicMint);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRound);

    g.setColour(resources.onyx);
    g.fillRoundedRectangle(ringRowBg,    cornerRound);
    g.fillRoundedRectangle(frqShftRowBg, cornerRound);
    g.fillRoundedRectangle(sAndHRowBg,   cornerRound);
}

void ModifierPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int modHeadingHeight  = 30;

    auto area              = getLocalBounds().reduced(sectionSpacerSize);
    int  modSectionGridWidth = area.getWidth() / 4;

    // Top header row: shared column labels (Tone / Pitch / Dry-Wet).
    auto headingsSpace = area.removeFromTop(modHeadingHeight);
    headingsSpace.removeFromLeft(modSectionGridWidth);    // skip the row-name column
    auto toneHeading  = headingsSpace.removeFromLeft(modSectionGridWidth);
    auto pitchHeading = headingsSpace.removeFromLeft(modSectionGridWidth);

    toneLabel  .setBounds(toneHeading);
    pitchLabel .setBounds(pitchHeading);
    dryWetLabel.setBounds(headingsSpace);

    int rowHeight = area.getHeight() / 3;

    auto subsections = area.reduced(sectionSpacerSize);
    auto ringRow    = subsections.removeFromTop(rowHeight).reduced(sectionSpacerSize);
    auto frqShftRow = subsections.removeFromTop(rowHeight).reduced(sectionSpacerSize);
    auto sAndHRow   = subsections                          .reduced(sectionSpacerSize);

    ringRowBg    = ringRow   .toFloat();
    frqShftRowBg = frqShftRow.toFloat();
    sAndHRowBg   = sAndHRow  .toFloat();

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
