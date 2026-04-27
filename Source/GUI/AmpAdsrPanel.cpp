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

    SetupSlider(this, oscAttackSlider,  juce::Slider::SliderStyle::LinearVertical,               primary, thumb, txt, true);
    SetupSlider(this, oscDecaySlider,   juce::Slider::SliderStyle::LinearVertical,               primary, thumb, txt, true);
    SetupSlider(this, oscSustainSlider, juce::Slider::SliderStyle::LinearVertical,               primary, thumb, txt, true);
    SetupSlider(this, oscReleaseSlider, juce::Slider::SliderStyle::LinearVertical,               primary, thumb, txt, true);
    SetupSlider(this, portaSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, primary, thumb, txt, false);
    SetupSlider(this, foldbackSlider,   juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt, false);

    oscAttackSlider .setLookAndFeel(res.dialLookAndFeel);
    oscDecaySlider  .setLookAndFeel(res.dialLookAndFeel);
    oscSustainSlider.setLookAndFeel(res.dialLookAndFeel);
    oscReleaseSlider.setLookAndFeel(res.dialLookAndFeel);
    portaSlider     .setLookAndFeel(res.dialLookAndFeel);
    foldbackSlider  .setLookAndFeel(res.dialLookAndFeel);

    SetupLabel(this, oscAttackLabel,  "A",                    txt, 18.0f);
    SetupLabel(this, oscDecayLabel,   "D",                    txt, 18.0f);
    SetupLabel(this, oscSustainLabel, "S",                    txt, 18.0f);
    SetupLabel(this, oscReleaseLabel, "R",                    txt, 18.0f);
    SetupLabel(this, portaLabel,      "Portamento",           txt, 17.0f);
    SetupLabel(this, foldbackLabel,   "Foldback\nDistortion", txt, 17.0f);

    attackAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_attack",    oscAttackSlider);
    decayAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_decay",     oscDecaySlider);
    sustainAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_sustain",   oscSustainSlider);
    releaseAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "amp_release",   oscReleaseSlider);
    portaAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "porta_time",    portaSlider);
    foldbackAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "foldback_dist", foldbackSlider);
}

void AmpAdsrPanel::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRound);

    g.setColour(resources.theme.background);
    g.fillRoundedRectangle(rotaryBg,  cornerRound);
    g.fillRoundedRectangle(slidersBg, cornerRound);
}

void AmpAdsrPanel::resized()
{
    constexpr int sectionSpacerSize  = 2;
    constexpr int rotaryLabelHeight  = 60;
    constexpr int oscADSRLabelHeight = 30;

    auto area = getLocalBounds().reduced(sectionSpacerSize);

    // Bottom third: portamento + foldback knobs.
    auto rotarySpace = area.removeFromBottom(area.getHeight() / 3).reduced(sectionSpacerSize * 2);
    rotaryBg = rotarySpace.toFloat();

    auto portaSpace      = rotarySpace.removeFromLeft(rotarySpace.getWidth() / 2);
    auto portaLabelSpace = portaSpace.removeFromTop(rotaryLabelHeight);

    portaSlider.setBounds(portaSpace);
    portaLabel .setBounds(portaLabelSpace);

    auto foldbackLabelSpace = rotarySpace.removeFromTop(rotaryLabelHeight);
    foldbackSlider.setBounds(rotarySpace);
    foldbackLabel .setBounds(foldbackLabelSpace);

    // Top two-thirds: ADSR sliders.
    auto slidersArea = area.reduced(sectionSpacerSize * 2);
    slidersBg = slidersArea.toFloat();

    int sliderWidth = slidersArea.getWidth() / 4;
    auto aSpace = slidersArea.removeFromLeft(sliderWidth);
    auto dSpace = slidersArea.removeFromLeft(sliderWidth);
    auto sSpace = slidersArea.removeFromLeft(sliderWidth);
    auto rSpace = slidersArea.removeFromLeft(sliderWidth);

    auto aLabelSpace = aSpace.removeFromTop(oscADSRLabelHeight);
    auto dLabelSpace = dSpace.removeFromTop(oscADSRLabelHeight);
    auto sLabelSpace = sSpace.removeFromTop(oscADSRLabelHeight);
    auto rLabelSpace = rSpace.removeFromTop(oscADSRLabelHeight);

    oscAttackSlider .setBounds(aSpace);
    oscDecaySlider  .setBounds(dSpace);
    oscSustainSlider.setBounds(sSpace);
    oscReleaseSlider.setBounds(rSpace);

    oscAttackLabel .setBounds(aLabelSpace);
    oscDecayLabel  .setBounds(dLabelSpace);
    oscSustainLabel.setBounds(sLabelSpace);
    oscReleaseLabel.setBounds(rLabelSpace);
}
