/*
  ==============================================================================

    AmpAdsrPanel.cpp

  ==============================================================================
*/

#include "AmpAdsrPanel.h"

#include "GuiHelpers.h"
#include "GUI/GuiHelpers.h"
#include "GUI/Format.h"
#include "GUI/MidiLearnAttachments.h"

//==============================================================================

AmpAdsrPanel::AmpAdsrPanel(GuiResources &res)
{
    setOpaque(false);
    
    auto primary = res.theme.primaryAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    GuiHelpers::SetupSlider(this, oscAttackSlider,  juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    GuiHelpers::SetupSlider(this, oscDecaySlider,   juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    GuiHelpers::SetupSlider(this, oscSustainSlider, juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);
    GuiHelpers::SetupSlider(this, oscReleaseSlider, juce::Slider::SliderStyle::LinearHorizontal, primary, thumb, txt);

    oscAttackSlider .setLookAndFeel(res.dialLookAndFeel);
    oscDecaySlider  .setLookAndFeel(res.dialLookAndFeel);
    oscSustainSlider.setLookAndFeel(res.dialLookAndFeel);
    oscReleaseSlider.setLookAndFeel(res.dialLookAndFeel);

    GuiHelpers::SetupSectionLabel(this, sectionLabel, "Amp", res.theme.textSecondary);

    const juce::Font::FontStyleFlags style = juce::Font::bold;
    GuiHelpers::SetupLabel(this, oscAttackLabel,  "A", txt, 16.0f, style, juce::Justification::centredRight);
    GuiHelpers::SetupLabel(this, oscDecayLabel,   "D", txt, 16.0f, style, juce::Justification::centredRight);
    GuiHelpers::SetupLabel(this, oscSustainLabel, "S", txt, 16.0f, style, juce::Justification::centredRight);
    GuiHelpers::SetupLabel(this, oscReleaseLabel, "R", txt, 16.0f, style, juce::Justification::centredRight);

    attackAtt  = MidiLearn::AttachSlider(*res.apvts, "amp_attack",  oscAttackSlider);
    decayAtt   = MidiLearn::AttachSlider(*res.apvts, "amp_decay",   oscDecaySlider);
    sustainAtt = MidiLearn::AttachSlider(*res.apvts, "amp_sustain", oscSustainSlider);
    releaseAtt = MidiLearn::AttachSlider(*res.apvts, "amp_release", oscReleaseSlider);

    Format::SetSliderTextFormat(oscAttackSlider,  Format::TimeSec);
    Format::SetSliderTextFormat(oscDecaySlider,   Format::TimeSec);
    Format::SetSliderTextFormat(oscSustainSlider, Format::Percent);
    Format::SetSliderTextFormat(oscReleaseSlider, Format::TimeSec);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    adsrVisual.SetColors(primary, bg, bgFade);

    adsrAttack  = res.apvts->getRawParameterValue("amp_attack");
    adsrDecay   = res.apvts->getRawParameterValue("amp_decay");
    adsrSustain = res.apvts->getRawParameterValue("amp_sustain");
    adsrRelease = res.apvts->getRawParameterValue("amp_release");

    adsrVisual.SetADSR(adsrAttack->load()
                       , adsrDecay->load()
                       , adsrSustain->load()
                       , adsrRelease->load());   

    addAndMakeVisible(adsrVisual);

    GuiHelpers::SetTip(oscAttackSlider,  "Main & Sub Attack");
    GuiHelpers::SetTip(oscDecaySlider,   "Main & Sub Decay");
    GuiHelpers::SetTip(oscSustainSlider, "Main & Sub Sustain");
    GuiHelpers::SetTip(oscReleaseSlider, "Main & Sub Release");
}

void AmpAdsrPanel::resized()
{
    auto bounds = getLocalBounds();
    sectionLabel.setBounds(bounds.removeFromTop(16));
    
    const int margin = bounds.proportionOfWidth(0.15f);
    auto visualArea  = bounds.removeFromTop(bounds.proportionOfHeight(0.5f))
                             .reduced(margin, 0);
    adsrVisual.setBounds(visualArea.removeFromLeft(visualArea.proportionOfWidth(0.97f)));
    
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
    adsrVisual.SetADSR(adsrAttack->load()
                       , adsrDecay->load()
                       , adsrSustain->load()
                       , adsrRelease->load());
}
