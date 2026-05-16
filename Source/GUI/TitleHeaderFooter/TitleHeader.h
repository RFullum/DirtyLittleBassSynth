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

    /// Thin delegators so the editor's keyboard-shortcut path doesn't have
    /// to reach through to PatchControls directly. Both forward to the
    /// matching PatchControls method.
    void TriggerPatchSave();
    void TriggerPatchSaveAs();

private:
    GuiResources &resources;

    /// Shows the app-level right-click menu (currently: tooltips on/off).
    /// Exposed as a method so the plugin-name Label can call it cleanly.
    void ShowAppMenu();

    TempoControls     tempoControls;
    MidiLearnControls midiLearnControls;
    PatchControls     patchControls;

    juce::TextButton  panicButton;

    /// Plugin name as an interactive Label so it can carry its own tooltip and
    /// receive right-clicks for the app-level menu. The Label is non-editable;
    /// we override its mouseDown via a small subclass.
    class TitleLabel : public juce::Label
    {
    public:
        explicit TitleLabel(TitleHeader &owner) : header(owner) {}
        void mouseDown(const juce::MouseEvent &e) override;
    private:
        TitleHeader &header;
    };

    TitleLabel pluginNameLabel { *this };

    juce::Rectangle<int> pluginNameRect;
    juce::Rectangle<int> taglineRect;
    juce::Rectangle<int> brandingRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleHeader)
};
