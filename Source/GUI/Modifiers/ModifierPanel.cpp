/*
  ==============================================================================

    ModifierPanel.cpp

  ==============================================================================
*/

#include "ModifierPanel.h"

#include "GUI/GuiHelpers.h"
#include "GuiHelpers.h"
#include "GUI/MidiLearnAttachments.h"

//==============================================================================

ModifierPanel::ModifierPanel(GuiResources &res)
: resources(res)
{
    setOpaque(false);
    
    auto primary = res.theme.primaryAccent;
    auto pink    = res.theme.pinkAccent;
    auto orange  = res.theme.orangeAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    GuiHelpers::SetupSlider(this, portaSlider,         juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, primary, thumb, txt);
    GuiHelpers::SetupSlider(this, foldbackSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    GuiHelpers::SetupSlider(this, ringToneSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,    thumb, txt);
    GuiHelpers::SetupSlider(this, ringPitchSlider,     juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,    thumb, txt);
    GuiHelpers::SetupSlider(this, ringDryWetSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,    thumb, txt);
    GuiHelpers::SetupSlider(this, frqShftPitchSlider,  juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    GuiHelpers::SetupSlider(this, frqShftDryWetSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    GuiHelpers::SetupSlider(this, sHPitchSlider,       juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    GuiHelpers::SetupSlider(this, sHDryWetSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);

    portaSlider        .setLookAndFeel(res.dialLookAndFeel);
    foldbackSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringToneSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringPitchSlider    .setLookAndFeel(res.dialLookAndFeel);
    ringDryWetSlider   .setLookAndFeel(res.dryWetLookAndFeel);
    frqShftPitchSlider .setLookAndFeel(res.dialLookAndFeel);
    frqShftDryWetSlider.setLookAndFeel(res.dryWetLookAndFeel);
    sHPitchSlider      .setLookAndFeel(res.dialLookAndFeel);
    sHDryWetSlider     .setLookAndFeel(res.dryWetLookAndFeel);
    
    ringDryWetSlider   .getProperties().set("snapAt50", true);
    frqShftDryWetSlider.getProperties().set("snapAt50", true);
    sHDryWetSlider     .getProperties().set("snapAt50", true);

    portaSlider        .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    foldbackSlider     .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    ringToneSlider     .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    ringPitchSlider    .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    ringDryWetSlider   .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    frqShftPitchSlider .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    frqShftDryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sHPitchSlider      .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sHDryWetSlider     .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    GuiHelpers::SetupSectionLabel(this, sectionLabel, "Modifiers", res.theme.textSecondary);

    GuiHelpers::SetupLabel(this, portaLabel,    "Portamento",           txt, 14.0f);
    GuiHelpers::SetupLabel(this, foldbackLabel, "Foldback\nDistortion", txt, 14.0f);
    GuiHelpers::SetupLabel(this, ringLabel,     "Ring Mod",             txt, 14.0f);
    GuiHelpers::SetupLabel(this, frqShftLabel,  "Freq Shift",           txt, 14.0f);
    GuiHelpers::SetupLabel(this, sHLabel,       "Sample & Hold",        txt, 14.0f);
    GuiHelpers::SetupLabel(this, toneLabel,     "Tone",                 txt, 12.0f);
    GuiHelpers::SetupLabel(this, pitchLabel,    "Pitch",                txt, 12.0f);
    GuiHelpers::SetupLabel(this, dryWetLabel,   "Dry/Wet",              txt, 12.0f);

    portaAtt         = MidiLearn::AttachSlider(*res.apvts, "porta_time",       portaSlider);
    foldbackAtt      = MidiLearn::AttachSlider(*res.apvts, "foldback_dist",    foldbackSlider);
    ringToneAtt      = MidiLearn::AttachSlider(*res.apvts, "ring_tone",        ringToneSlider);
    ringPitchAtt     = MidiLearn::AttachSlider(*res.apvts, "ring_mod_pitch",   ringPitchSlider);
    ringDryWetAtt    = MidiLearn::AttachSlider(*res.apvts, "ring_mod_mix",     ringDryWetSlider);
    frqShftPitchAtt  = MidiLearn::AttachSlider(*res.apvts, "freq_shift_pitch", frqShftPitchSlider);
    frqShftDryWetAtt = MidiLearn::AttachSlider(*res.apvts, "freq_shift_mix",   frqShftDryWetSlider);
    sHPitchAtt       = MidiLearn::AttachSlider(*res.apvts, "sandh_pitch",      sHPitchSlider);
    sHDryWetAtt      = MidiLearn::AttachSlider(*res.apvts, "sandh_mix",        sHDryWetSlider);

    auto styleModeButton = [&](juce::TextButton &btn, juce::Colour onTextColor)
    {
        btn.setClickingTogglesState(true);
        btn.setColour(juce::TextButton::buttonColourId,   res.theme.structure);
        btn.setColour(juce::TextButton::buttonOnColourId, primary.withAlpha(0.25f));
        btn.setColour(juce::TextButton::textColourOnId,   onTextColor);
        btn.setColour(juce::TextButton::textColourOffId,  res.theme.textSecondary);
        addAndMakeVisible(btn);
    };

    styleModeButton(portaOnButton,     primary);
    styleModeButton(portaLegatoButton, primary);

    portaOnAtt     = MidiLearn::AttachButton(*res.apvts, "porta_on",     portaOnButton);
    portaLegatoAtt = MidiLearn::AttachButton(*res.apvts, "porta_legato", portaLegatoButton);

    res.apvts->addParameterListener("porta_on",     this);
    res.apvts->addParameterListener("porta_legato", this);

    RefreshPortaLook();

    GuiHelpers::SetTip(portaSlider,         "Portamento glide time");
    GuiHelpers::SetTip(foldbackSlider,      "Foldback Distortion Amount");
    GuiHelpers::SetTip(ringToneSlider,      "Ring Mod Tone");
    GuiHelpers::SetTip(ringPitchSlider,     "Ring Mod Pitch");
    GuiHelpers::SetTip(ringDryWetSlider,    "Ring Mod Dry/Wet");
    GuiHelpers::SetTip(frqShftPitchSlider,  "Frequency Shift Pitch");
    GuiHelpers::SetTip(frqShftDryWetSlider, "Frequency Shift Dry/Wet");
    GuiHelpers::SetTip(sHPitchSlider,       "Sample and Hold Pitch");
    GuiHelpers::SetTip(sHDryWetSlider,      "Sample and Hold Dry/Wet");
    GuiHelpers::SetTip(portaOnButton,       "Portamento On/Off");
    GuiHelpers::SetTip(portaLegatoButton,   "Portamento\nAlways: glides every note\nLegato: only glides on overlapping notes"); 
}

ModifierPanel::~ModifierPanel()
{
    resources.apvts->removeParameterListener("porta_on",     this);
    resources.apvts->removeParameterListener("porta_legato", this);
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

    portaLabel.setBounds(portaArea.removeFromTop(rowLabelH));

    static constexpr int portaBtnRowHeight = 18;
    static constexpr int portaBtnGap       = 4;
    static constexpr int portaBtnRowInset  = 12;

    auto portaBtnRow = portaArea.removeFromBottom(portaBtnRowHeight)
                                .reduced(portaBtnRowInset, 0);

    const int btnWidth = (portaBtnRow.getWidth() - portaBtnGap) / 2;
    portaOnButton    .setBounds(portaBtnRow.removeFromLeft(btnWidth));
    portaBtnRow      .removeFromLeft(portaBtnGap);
    portaLegatoButton.setBounds(portaBtnRow.removeFromLeft(btnWidth));

    portaSlider.setBounds(portaArea.withSizeKeepingCentre(topSliderSize, topSliderSize));

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
    ringPitchSlider .setBounds(ringModRow.removeFromLeft(modSectionGridWidth));
    ringDryWetSlider.setBounds(ringModRow.removeFromLeft(modSectionGridWidth));
    
    frqShftLabel       .setBounds(freqShftRow.removeFromLeft(modSectionGridWidth));
    freqShftRow        .removeFromLeft(modSectionGridWidth);
    frqShftPitchSlider .setBounds(freqShftRow.removeFromLeft(modSectionGridWidth));
    frqShftDryWetSlider.setBounds(freqShftRow.removeFromLeft(modSectionGridWidth));
    
    sHLabel       .setBounds(sAndHRow.removeFromLeft(modSectionGridWidth));
    sAndHRow      .removeFromLeft(modSectionGridWidth);
    sHPitchSlider .setBounds(sAndHRow.removeFromLeft(modSectionGridWidth));
    sHDryWetSlider.setBounds(sAndHRow.removeFromLeft(modSectionGridWidth));
}

void ModifierPanel::parameterChanged(const juce::String &parameterID, float newValue)
{
    juce::ignoreUnused(newValue);

    if (parameterID == "porta_on" || parameterID == "porta_legato")
    {
        // Listener may fire on the audio thread; bounce to the message thread.
        juce::Component::SafePointer<ModifierPanel> self(this);
        juce::MessageManager::callAsync([self]()
        {
            if (self != nullptr)
                self->RefreshPortaLook();
        });
    }
}

void ModifierPanel::RefreshPortaLook()
{
    const bool on     = portaOnButton    .getToggleState();
    const bool legato = portaLegatoButton.getToggleState();

    portaOnButton    .setButtonText(on     ? "ON"     : "OFF");
    portaLegatoButton.setButtonText(legato ? "LEGATO" : "ALWAYS");

    // Dim when off; slider stays interactive so time can be set before toggling on.
    const float a = on ? 1.0f : 0.5f;
    portaLabel .setAlpha(a);
    portaSlider.setAlpha(a);
}
