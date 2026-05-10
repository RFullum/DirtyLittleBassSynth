/*
  ==============================================================================

    MidiLearnControls.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//==============================================================================

/// Title-header MIDI Learn cluster: a LEARN toggle that's always visible plus
/// a CLEAR MAPS button that only appears while learn mode is active. Both
/// drive `MidiLearnManager` directly; Update() keeps the toggle state in sync
/// with the manager so audio-thread transitions (Armed → Listening on a
/// successful bind) reflect in the UI.
class MidiLearnControls
    : public juce::Component
{
public:
    MidiLearnControls(GuiResources &resources);
    ~MidiLearnControls() override = default;

    void resized() override;

    /// Mirror the manager's state into the LEARN button toggle and toggle
    /// CLEAR MAPS visibility. Call from the editor's timer.
    void Update();

private:
    GuiResources &resources;

    juce::TextButton learnButton;
    juce::TextButton clearMapsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiLearnControls)
};
