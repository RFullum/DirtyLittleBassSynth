/*
  ==============================================================================

    GuiResources.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"
#include "ScopeBuffer.h"
#include "TempoSnapshot.h"
#include "MidiLearnManager.h"

//============================================================

/// Editor-owned resources shared across all panels: the parameter tree, look-and-feels,
/// active theme, and audio→UI feeds (scope, tempo). Each panel takes a non-const ref
/// to one of these in its constructor.
struct GuiResources
{
    juce::AudioProcessorValueTreeState *apvts;
    juce::LookAndFeel                  *dialLookAndFeel;
    juce::LookAndFeel                  *dryWetLookAndFeel;
    const Palette::Theme               &theme;
    const ScopeBuffer                  *scopeBuffer    = nullptr;
    const TempoSnapshot                *tempoSnapshot  = nullptr;

    /// True when the plugin is running as the standalone app. Used by the title
    /// header to allow drag-edit of the fallback BPM only when no host is present.
    bool                                isStandalone   = false;

    /// MIDI Learn manager owned by the processor. Used by the title header for
    /// the LEARN / CLEAR MAPS buttons and by the glass-pane overlay for arming.
    MidiLearnManager                   *midiLearnManager = nullptr;
};
