/*
  ==============================================================================

    AmpAdsrPanel.cpp

  ==============================================================================
*/

#include "AmpAdsrPanel.h"
#include "GuiHelpers.h"

//============================================================

AmpAdsrPanel::AmpAdsrPanel(GuiResources &res)
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

    DLBS::SetupLabel(this, oscAttackLabel,  "A", txt, 16.0f, juce::Justification::centredRight);
    DLBS::SetupLabel(this, oscDecayLabel,   "D", txt, 16.0f, juce::Justification::centredRight);
    DLBS::SetupLabel(this, oscSustainLabel, "S", txt, 16.0f, juce::Justification::centredRight);
    DLBS::SetupLabel(this, oscReleaseLabel, "R", txt, 16.0f, juce::Justification::centredRight);

    attackAtt  = DLBS::AttachSlider(*res.apvts, "amp_attack",  oscAttackSlider);
    decayAtt   = DLBS::AttachSlider(*res.apvts, "amp_decay",   oscDecaySlider);
    sustainAtt = DLBS::AttachSlider(*res.apvts, "amp_sustain", oscSustainSlider);
    releaseAtt = DLBS::AttachSlider(*res.apvts, "amp_release", oscReleaseSlider);

    DLBS::SetSliderTextFormat(oscAttackSlider,  DLBS::FormatTime);
    DLBS::SetSliderTextFormat(oscDecaySlider,   DLBS::FormatTime);
    DLBS::SetSliderTextFormat(oscSustainSlider, DLBS::FormatPercent);
    DLBS::SetSliderTextFormat(oscReleaseSlider, DLBS::FormatTime);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    adsrVisual.SetColors(primary, bg, bgFade);
    adsrVisual.Init(res.apvts->getRawParameterValue("amp_attack")
                    , res.apvts->getRawParameterValue("amp_decay")
                    , res.apvts->getRawParameterValue("amp_sustain")
                    , res.apvts->getRawParameterValue("amp_release"));

    addAndMakeVisible(adsrVisual);
}

void AmpAdsrPanel::resized()
{
    auto bounds = getLocalBounds();
    sectionLabel.setBounds(bounds.removeFromTop(16));
    
    const int margin = bounds.proportionOfWidth(0.15f);
    adsrVisual.setBounds(bounds.removeFromTop(bounds.proportionOfHeight(0.5f))
                               .reduced(margin, 0));
    
    const int rowH = bounds.proportionOfHeight(0.25f);
    auto adsrRow   = [&](juce::Slider &slider, juce::Label &label)
    {
        auto row = bounds.removeFromTop(rowH);
        label .setBounds(row.removeFromLeft(row.proportionOfWidth(0.1375f)));
        row   .removeFromRight(18);
        slider.setBounds(row);
    };

    adsrRow(oscAttackSlider,  oscAttackLabel);
    adsrRow(oscDecaySlider,   oscDecayLabel);
    adsrRow(oscSustainSlider, oscSustainLabel);
    adsrRow(oscReleaseSlider, oscReleaseLabel);
}

void AmpAdsrPanel::Update()
{
    adsrVisual.Update();
}
