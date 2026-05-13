/*
  ==============================================================================

    MidiLearnControls.cpp

  ==============================================================================
*/

#include "MidiLearnControls.h"

//==============================================================================

MidiLearnControls::MidiLearnControls(GuiResources &res)
: resources(res)
{
    setOpaque(false);
    
    // === LEARN ===
    // Toggles between Idle and Listening on the manager. Toggle state mirrors
    // manager.GetState() via Update(), so any source flipping the state
    // (audio thread, future shortcuts) keeps the button in sync.
    learnButton.setButtonText            ("LEARN");
    learnButton.setClickingTogglesState  (true);
    learnButton.setColour                (juce::TextButton::buttonColourId,   res.theme.structure);
    learnButton.setColour                (juce::TextButton::buttonOnColourId, res.theme.secondaryAccent.withAlpha(0.25f));
    learnButton.setColour                (juce::TextButton::textColourOnId,   res.theme.secondaryAccent);
    learnButton.setColour                (juce::TextButton::textColourOffId,  res.theme.textSecondary);
    learnButton.onClick = [this]()
    {
        if (resources.midiLearnManager == nullptr)
            return;

        if (learnButton.getToggleState())
            resources.midiLearnManager->EnterListening();
        else
            resources.midiLearnManager->ExitLearn();
    };
    addAndMakeVisible(learnButton);

    // === CLEAR MAPS ===
    // Visible only in learn mode. Wipes every CC binding.
    clearMapsButton.setButtonText("CLEAR MAPS");
    clearMapsButton.setColour    (juce::TextButton::buttonColourId,  res.theme.structure);
    clearMapsButton.setColour    (juce::TextButton::textColourOffId, res.theme.orangeAccent);
    clearMapsButton.onClick = [this]()
    {
        if (resources.midiLearnManager != nullptr)
            resources.midiLearnManager->ClearAllMappings();
    };
    addChildComponent(clearMapsButton);   // hidden by default; Update() shows it in learn mode
}

void MidiLearnControls::resized()
{
    // Layout matches the original TitleHeader right-to-left placement: LEARN
    // sits at the right edge of our local bounds, CLEAR MAPS to its left with
    // a 6px gap. The two buttons occupy the full vertical extent.
    constexpr int learnBtnWidth     = 56;
    constexpr int learnBtnGap       = 6;
    constexpr int clearMapsBtnWidth = 88;

    auto bounds = getLocalBounds();

    learnButton    .setBounds(bounds.getRight() - learnBtnWidth
                              , bounds.getY()
                              , learnBtnWidth
                              , bounds.getHeight());
    clearMapsButton.setBounds(bounds.getRight() - learnBtnWidth - learnBtnGap - clearMapsBtnWidth
                              , bounds.getY()
                              , clearMapsBtnWidth
                              , bounds.getHeight());
}

void MidiLearnControls::Update()
{
    if (resources.midiLearnManager == nullptr)
        return;

    const bool learning = (resources.midiLearnManager->GetState() != MidiLearnManager::State::Idle);

    if (learnButton.getToggleState() != learning)
        learnButton.setToggleState(learning, juce::dontSendNotification);

    if (clearMapsButton.isVisible() != learning)
        clearMapsButton.setVisible(learning);
}
