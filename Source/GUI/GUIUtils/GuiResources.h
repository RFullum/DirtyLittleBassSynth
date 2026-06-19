/*
  ==============================================================================

    GuiResources.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"
#include "DSP/Utils/ScopeBuffer.h"
#include "TempoSnapshot.h"
#include "State/MidiLearnManager.h"
#include "State/PatchManager.h"


//==============================================================================

// Editor-owned bundle of shared references handed to every panel by const ref.
struct GuiResources
{
    juce::AudioProcessorValueTreeState *apvts             = nullptr;
    juce::LookAndFeel                  *dialLookAndFeel   = nullptr;
    juce::LookAndFeel                  *dryWetLookAndFeel = nullptr;
    const Palette::Theme               &theme;
    const ScopeBuffer                  *scopeBuffer    = nullptr;
    const TempoSnapshot                *tempoSnapshot  = nullptr;

    // Drives standalone-only branches (BPM drag-edit, CC Echo, etc.).
    bool                                isStandalone   = false;

    MidiLearnManager                   *midiLearnManager = nullptr;
    PatchManager                       *patchManager     = nullptr;

    std::function<void()>               midiPanic;

    // The editor's timer reconciles TooltipWindow lifetime against the getter.
    std::function<bool()>               getTooltipsEnabled;
    std::function<void(bool)>           setTooltipsEnabled;

    std::function<bool()>               getCcEchoEnabled;
    std::function<void(bool)>           setCcEchoEnabled;

    // Per-block voice snapshots for the FilterPanel's animated visualiser.
    const std::atomic<float>           *filtEnvDisplay = nullptr;  // 0..1
    const std::atomic<float>           *filtLFODisplay = nullptr;  // -1..1
};
