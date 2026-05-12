#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiResources.h"
#include "OtherLookAndFeel.h"
#include "SourcesColumn.h"
#include "FilterColumn.h"
#include "ModifiersColumn.h"
#include "MasterColumn.h"
#include "TitleHeaderFooter/TitleHeader.h"
#include "TitleHeaderFooter/TitleFooter.h"
#include "MIDI/MidiLearnOverlay.h"

//==============================================================================

class DirtyLittleBassSynthAudioProcessorEditor
    : public  juce::AudioProcessorEditor
    , private juce::Timer
    , private juce::KeyListener
{
public:
    DirtyLittleBassSynthAudioProcessorEditor(DirtyLittleBassSynthAudioProcessor &);
    ~DirtyLittleBassSynthAudioProcessorEditor();

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void timerCallback() override;

    /// Standalone-only Cmd+S / Cmd+Shift+S handling. In DAW contexts the host
    /// intercepts Cmd+S for project save, so we gate on isStandalone and
    /// return false otherwise (letting the host see the event).
    bool keyPressed(const juce::KeyPress &key, juce::Component *originator) override;

    DirtyLittleBassSynthAudioProcessor &processor;

    std::vector<juce::Rectangle<int>> dividers;

    OtherLookAndFeel dialLookAndFeel;
    OtherLookAndFeel dryWetLookAndFeel;

    GuiResources resources;

    TitleHeader titleHeader;
    TitleFooter titleFooter;

    SourcesColumn   sourcesColumn;
    FilterColumn    filterColumn;
    ModifiersColumn modifiersColumn;
    MasterColumn    masterColumn;

    // Last in the declaration list so it's added on top of every other child
    // and intercepts clicks first when learn mode is active.
    MidiLearnOverlay midiLearnOverlay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirtyLittleBassSynthAudioProcessorEditor)
};
