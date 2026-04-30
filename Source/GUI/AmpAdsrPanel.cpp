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
    auto orange  = res.theme.orangeAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    SetupSlider(this, oscAttackSlider,  juce::Slider::SliderStyle::LinearHorizontal,             primary, thumb, txt);
    SetupSlider(this, oscDecaySlider,   juce::Slider::SliderStyle::LinearHorizontal,             primary, thumb, txt);
    SetupSlider(this, oscSustainSlider, juce::Slider::SliderStyle::LinearHorizontal,             primary, thumb, txt);
    SetupSlider(this, oscReleaseSlider, juce::Slider::SliderStyle::LinearHorizontal,             primary, thumb, txt);
    SetupSlider(this, portaSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, primary, thumb, txt);
    SetupSlider(this, foldbackSlider,   juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);

    oscAttackSlider .setLookAndFeel(res.dialLookAndFeel);
    oscDecaySlider  .setLookAndFeel(res.dialLookAndFeel);
    oscSustainSlider.setLookAndFeel(res.dialLookAndFeel);
    oscReleaseSlider.setLookAndFeel(res.dialLookAndFeel);
    portaSlider     .setLookAndFeel(res.dialLookAndFeel);
    foldbackSlider  .setLookAndFeel(res.dialLookAndFeel);

    // TODO: change dash to a dot? \xc2\xb7
    SetupSectionLabel(this, sectionLabel, "Amp - Drive", res.theme.textSecondary);

    SetupLabel(this, oscAttackLabel,  "A",                    txt, 16.0f);
    SetupLabel(this, oscDecayLabel,   "D",                    txt, 16.0f);
    SetupLabel(this, oscSustainLabel, "S",                    txt, 16.0f);
    SetupLabel(this, oscReleaseLabel, "R",                    txt, 16.0f);
    SetupLabel(this, portaLabel,      "Portamento",           txt, 17.0f);
    SetupLabel(this, foldbackLabel,   "Foldback\nDistortion", txt, 17.0f);

    attackAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_attack",    oscAttackSlider);
    decayAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_decay",     oscDecaySlider);
    sustainAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_sustain",   oscSustainSlider);
    releaseAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_release",   oscReleaseSlider);
    portaAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "porta_time",    portaSlider);
    foldbackAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "foldback_dist", foldbackSlider);

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
    constexpr int rotaryLabelHeight = 60;
    constexpr int adsrLabelWidth    = 18;

    auto area = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(area.removeFromTop(16).reduced(8, 0));

    // Bottom third: portamento + foldback knobs.
    auto rotarySpace = area.removeFromBottom(area.getHeight() / 3).reduced(sectionSpacerSize * 2);

    auto portaSpace      = rotarySpace.removeFromLeft(rotarySpace.getWidth() / 2);
    auto portaLabelSpace = portaSpace.removeFromTop(rotaryLabelHeight);

    portaSlider.setBounds(portaSpace);
    portaLabel .setBounds(portaLabelSpace);

    auto foldbackLabelSpace = rotarySpace.removeFromTop(rotaryLabelHeight);
    foldbackSlider.setBounds(rotarySpace);
    foldbackLabel .setBounds(foldbackLabelSpace);

    // Top two-thirds: envelope visual on top, then 4 stacked horizontal ADSR
    // rows, each with letter label on the left and the slider filling the rest.
    auto slidersArea = area.reduced(sectionSpacerSize * 2);

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

    layoutRow(oscAttackSlider,  oscAttackLabel);
    layoutRow(oscDecaySlider,   oscDecayLabel);
    layoutRow(oscSustainSlider, oscSustainLabel);
    layoutRow(oscReleaseSlider, oscReleaseLabel);
}

void AmpAdsrPanel::Update()
{
    adsrVisual.Update();
}
