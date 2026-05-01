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
    using namespace dlbs;

    auto primary = res.theme.primaryAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    SetupSlider(this, oscAttackSlider,  juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    SetupSlider(this, oscDecaySlider,   juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    SetupSlider(this, oscSustainSlider, juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    SetupSlider(this, oscReleaseSlider, juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);

    oscAttackSlider .setLookAndFeel(res.dialLookAndFeel);
    oscDecaySlider  .setLookAndFeel(res.dialLookAndFeel);
    oscSustainSlider.setLookAndFeel(res.dialLookAndFeel);
    oscReleaseSlider.setLookAndFeel(res.dialLookAndFeel);

    SetupSectionLabel(this, sectionLabel, "Amp", res.theme.textSecondary);

    SetupLabel(this, oscAttackLabel,  "A", txt, 16.0f);
    SetupLabel(this, oscDecayLabel,   "D", txt, 16.0f);
    SetupLabel(this, oscSustainLabel, "S", txt, 16.0f);
    SetupLabel(this, oscReleaseLabel, "R", txt, 16.0f);

    attackAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_attack",  oscAttackSlider);
    decayAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_decay",   oscDecaySlider);
    sustainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_sustain", oscSustainSlider);
    releaseAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_release", oscReleaseSlider);

    SetSliderTextFormat(oscAttackSlider,  FormatTime);
    SetSliderTextFormat(oscDecaySlider,   FormatTime);
    SetSliderTextFormat(oscSustainSlider, FormatPercent);
    SetSliderTextFormat(oscReleaseSlider, FormatTime);

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

    auto area = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(area.removeFromTop(16).reduced(8, 0));

    // Envelope visual on top, then 4 stacked horizontal ADSR rows, each with
    // letter label on the left and the slider filling the rest.
    auto slidersArea = area.reduced(sectionSpacerSize * 2);

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
