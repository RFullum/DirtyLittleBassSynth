/*
  ==============================================================================

    MidiLearnOverlay.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"
#include "MidiLearnManager.h"

//============================================================

/// Glass-pane overlay sized over the editor's body area. Hidden in idle mode;
/// when MIDI Learn is active it becomes visible and intercepts every mouse
/// click that would otherwise reach a learnable control.
///
/// On click, the overlay temporarily disables its own hit-testing, asks the
/// parent (editor) what would have been clicked, walks up the resulting
/// component's parent chain until it finds a `paramID` property, and tells
/// the MidiLearnManager to arm that param. The next incoming CC will then
/// bind to it.
///
/// The hover / armed-pulse / CC# badge layers are added in a later step;
/// this class only handles click capture + the global tint indicator for now.
class MidiLearnOverlay
    : public juce::Component
{
public:
    MidiLearnOverlay(MidiLearnManager &manager, const Palette::Theme &theme);

    void paint    (juce::Graphics &)        override;
    void mouseDown(const juce::MouseEvent &) override;

private:
    /// Returns the topmost learnable component under `editorPos` (in editor
    /// coordinates), or nullptr if no `paramID`-tagged ancestor was found.
    juce::Component *FindLearnableAt(juce::Point<int> editorPos);

    /// Walks up `start`'s parent chain looking for a component whose
    /// properties contain a `paramID` key. Stops at the editor.
    static juce::Component *WalkUpForParamID(juce::Component *start);

    MidiLearnManager      &manager;
    const Palette::Theme  &theme;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiLearnOverlay)
};
