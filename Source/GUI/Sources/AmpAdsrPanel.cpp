/*
  ==============================================================================

    AmpAdsrPanel.cpp

  ==============================================================================
*/

#include "AmpAdsrPanel.h"
#include "GuiHelpers.h"

//============================================================

AmpAdsrPanel::AmpAdsrPanel(GuiResources &res)
: resources(res)
{
    auto primary = res.theme.primaryAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    DLBS::SetupSlider(this, oscAttackSlider,  juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    DLBS::SetupSlider(this, oscDecaySlider,   juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    DLBS::SetupSlider(this, oscSustainSlider, juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    DLBS::SetupSlider(this, oscReleaseSlider, juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);

    oscAttackSlider .setLookAndFeel(res.dialLookAndFeel);
    oscDecaySlider  .setLookAndFeel(res.dialLookAndFeel);
    oscSustainSlider.setLookAndFeel(res.dialLookAndFeel);
    oscReleaseSlider.setLookAndFeel(res.dialLookAndFeel);

    DLBS::SetupSectionLabel(this, sectionLabel, "Amp", res.theme.textSecondary);

    DLBS::SetupLabel(this, oscAttackLabel,  "A", txt, 16.0f);
    DLBS::SetupLabel(this, oscDecayLabel,   "D", txt, 16.0f);
    DLBS::SetupLabel(this, oscSustainLabel, "S", txt, 16.0f);
    DLBS::SetupLabel(this, oscReleaseLabel, "R", txt, 16.0f);

    attackAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_attack",  oscAttackSlider);
    decayAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_decay",   oscDecaySlider);
    sustainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_sustain", oscSustainSlider);
    releaseAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_release", oscReleaseSlider);

    DLBS::SetSliderTextFormat(oscAttackSlider,  DLBS::FormatTime);
    DLBS::SetSliderTextFormat(oscDecaySlider,   DLBS::FormatTime);
    DLBS::SetSliderTextFormat(oscSustainSlider, DLBS::FormatPercent);
    DLBS::SetSliderTextFormat(oscReleaseSlider, DLBS::FormatTime);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    adsrVisual.SetColors(primary, bg, bgFade);
    adsrVisual.Init(res.apvts->getRawParameterValue("amp_attack"),
                    res.apvts->getRawParameterValue("amp_decay"),
                    res.apvts->getRawParameterValue("amp_sustain"),
                    res.apvts->getRawParameterValue("amp_release"));

    addAndMakeVisible(adsrVisual);
}

void AmpAdsrPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int adsrLabelWidth    = 18;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(bounds.removeFromTop(16).reduced(8, 0));

    // Envelope visual on top, then 4 stacked horizontal ADSR rows, each with
    // letter label on the left and the slider filling the rest.
    auto slidersArea = bounds.reduced(sectionSpacerSize * 2);

    const int visualHeight = slidersArea.getHeight() * 2 / 5;
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

    layoutRow(oscAttackSlider,  oscAttackLabel);
    layoutRow(oscDecaySlider,   oscDecayLabel);
    layoutRow(oscSustainSlider, oscSustainLabel);
    layoutRow(oscReleaseSlider, oscReleaseLabel);
}

void AmpAdsrPanel::Update()
{
    adsrVisual.Update();
}
