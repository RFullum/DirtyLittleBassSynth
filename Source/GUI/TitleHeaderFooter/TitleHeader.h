/*
  ==============================================================================

    TitleHeader.h
    Created: 12 Jan 2021 11:10:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"
#include "GuiResources.h"
#include "TempoControls.h"
#include "MidiLearnControls.h"
#include "PatchControls.h"

//==============================================================================

/// Top-of-window header strip. The header itself paints the static brand text
/// (plugin name + tagline on the left, FULLUMMUSIC on the right) and lays out
/// three interactive child clusters: the centred TempoControls, the
/// MidiLearnControls (LEARN / CLEAR MAPS), and the PatchControls
/// (INIT PATCH label + prev/next arrows).
class TitleHeader
    : public juce::Component
{
public:
    TitleHeader(GuiResources &resources);
    ~TitleHeader() override = default;

    void paint  (juce::Graphics &) override;
    void resized()                  override;

    /// Drives the timer-fed children (tempo display, MIDI Learn button sync).
    /// Call from the editor's timer.
    void Update();

private:
    GuiResources &resources;

    TempoControls     tempoControls;
    MidiLearnControls midiLearnControls;
    PatchControls     patchControls;

    juce::Rectangle<int> pluginNameRect;
    juce::Rectangle<int> taglineRect;
    juce::Rectangle<int> brandingRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleHeader)
};
