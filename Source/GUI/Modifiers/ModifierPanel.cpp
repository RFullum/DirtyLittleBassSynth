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
    setOpaque(false);
    
    auto primary = res.theme.primaryAccent;
    auto pink    = res.theme.pinkAccent;
    auto orange  = res.theme.orangeAccent;
    auto thumb   = res.theme.textPrimary;
    auto txt     = res.theme.textPrimary;

    DLBS::SetupSlider(this, portaSlider,         juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, primary, thumb, txt);
    DLBS::SetupSlider(this, foldbackSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    DLBS::SetupSlider(this, ringToneSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,    thumb, txt);
    DLBS::SetupSlider(this, ringPitchSlider,     juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,    thumb, txt);
    DLBS::SetupSlider(this, ringDryWetSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, pink,    thumb, txt);
    DLBS::SetupSlider(this, frqShftPitchSlider,  juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    DLBS::SetupSlider(this, frqShftDryWetSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    DLBS::SetupSlider(this, sHPitchSlider,       juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);
    DLBS::SetupSlider(this, sHDryWetSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, orange,  thumb, txt);

    portaSlider        .setLookAndFeel(res.dialLookAndFeel);
    foldbackSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringToneSlider     .setLookAndFeel(res.dialLookAndFeel);
    ringPitchSlider    .setLookAndFeel(res.dialLookAndFeel);
    ringDryWetSlider   .setLookAndFeel(res.dryWetLookAndFeel);
    frqShftPitchSlider .setLookAndFeel(res.dialLookAndFeel);
    frqShftDryWetSlider.setLookAndFeel(res.dryWetLookAndFeel);
    sHPitchSlider      .setLookAndFeel(res.dialLookAndFeel);
    sHDryWetSlider     .setLookAndFeel(res.dryWetLookAndFeel);
    
    // Dry/Wet rotaries get a 50% tick + brighter highlight when near centre,
    // since 50/50 is a meaningful balance point on those controls.
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

    DLBS::SetupSectionLabel(this, sectionLabel, "Modifiers", res.theme.textSecondary);

    DLBS::SetupLabel(this, portaLabel,    "Portamento",           txt, 14.0f);
    DLBS::SetupLabel(this, foldbackLabel, "Foldback\nDistortion", txt, 14.0f);
    DLBS::SetupLabel(this, ringLabel,     "Ring Mod",             txt, 14.0f);
    DLBS::SetupLabel(this, frqShftLabel,  "Freq Shift",           txt, 14.0f);
    DLBS::SetupLabel(this, sHLabel,       "Sample & Hold",        txt, 14.0f);
    DLBS::SetupLabel(this, toneLabel,     "Tone",                 txt, 12.0f);
    DLBS::SetupLabel(this, pitchLabel,    "Pitch",                txt, 12.0f);
    DLBS::SetupLabel(this, dryWetLabel,   "Dry/Wet",              txt, 12.0f);

    portaAtt         = DLBS::AttachSlider(*res.apvts, "porta_time",       portaSlider);
    foldbackAtt      = DLBS::AttachSlider(*res.apvts, "foldback_dist",    foldbackSlider);
    ringToneAtt      = DLBS::AttachSlider(*res.apvts, "ring_tone",        ringToneSlider);
    ringPitchAtt     = DLBS::AttachSlider(*res.apvts, "ring_mod_pitch",   ringPitchSlider);
    ringDryWetAtt    = DLBS::AttachSlider(*res.apvts, "ring_mod_mix",     ringDryWetSlider);
    frqShftPitchAtt  = DLBS::AttachSlider(*res.apvts, "freq_shift_pitch", frqShftPitchSlider);
    frqShftDryWetAtt = DLBS::AttachSlider(*res.apvts, "freq_shift_mix",   frqShftDryWetSlider);
    sHPitchAtt       = DLBS::AttachSlider(*res.apvts, "sandh_pitch",      sHPitchSlider);
    sHDryWetAtt      = DLBS::AttachSlider(*res.apvts, "sandh_mix",        sHDryWetSlider);

    // === Portamento mode buttons: ON/OFF + ALWAYS/LEGATO ===
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

    portaOnAtt     = DLBS::AttachButton(*res.apvts, "porta_on",     portaOnButton);
    portaLegatoAtt = DLBS::AttachButton(*res.apvts, "porta_legato", portaLegatoButton);

    res.apvts->addParameterListener("porta_on",     this);
    res.apvts->addParameterListener("porta_legato", this);

    // Initial button labels + alpha state.
    RefreshPortaLook();
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

    // Two mode buttons sit at the bottom of portaArea; the rotary fills the rest.
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

    // Portamento label + slider dim when off; slider stays interactive so the
    // user can still tweak the time value without flipping the toggle.
    const float a = on ? 1.0f : 0.5f;
    portaLabel .setAlpha(a);
    portaSlider.setAlpha(a);
}
