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

class TitleHeader
    : public juce::Component
{
public:
    TitleHeader(GuiResources &resources);
    ~TitleHeader() override = default;

    void paint  (juce::Graphics &) override;
    void resized()                  override;

    void Update();

    void TriggerPatchSave();
    void TriggerPatchSaveAs();

private:
    class TitleLabel
        : public juce::Label
    {
    public:
        explicit TitleLabel(TitleHeader &owner)
            : header(owner)
        {}
        
        void mouseDown(const juce::MouseEvent &e) override;
    private:
        TitleHeader &header;
    };
    
    void ShowAppMenu();
    
    GuiResources &resources;
    
    TempoControls     tempoControls;
    MidiLearnControls midiLearnControls;
    PatchControls     patchControls;

    juce::TextButton  panicButton;

    TitleLabel pluginNameLabel { *this };

    juce::Rectangle<int> pluginNameRect;
    juce::Rectangle<int> taglineRect;
    juce::Rectangle<int> brandingRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleHeader)
};
