#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiResources.h"
#include "OtherLookAndFeel.h"
#include "PopupLookAndFeel.h"
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

    // Standalone-only Cmd+S / Cmd+Shift+S handling.
    bool keyPressed(const juce::KeyPress &key, juce::Component *originator) override;

    DirtyLittleBassSynthAudioProcessor &processor;

    std::vector<juce::Rectangle<int>> dividers;

    OtherLookAndFeel dialLookAndFeel;
    OtherLookAndFeel dryWetLookAndFeel;
    PopupLookAndFeel popupLookAndFeel { Palette::DefaultTheme };

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

    // Created on demand when tooltips are enabled AND we're not in MIDI Learn
    // mode. Destroying it suppresses tooltip rendering without touching the
    // setTooltip() calls scattered through the panels.
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    void CreateTooltipWindow();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirtyLittleBassSynthAudioProcessorEditor)
};
