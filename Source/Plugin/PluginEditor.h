#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiResources.h"
#include "OtherLookAndFeel.h"
#include "TitleHeader.h"
#include "OscPanel.h"
#include "AmpAdsrPanel.h"
#include "ModifierPanel.h"
#include "FilterPanel.h"
#include "FilterAdsrPanel.h"
#include "LfoPanel.h"
#include "MasterPanel.h"

//==============================================================================

class DirtyLittleBassSynthAudioProcessorEditor
: public  juce::AudioProcessorEditor
, private juce::Timer
{
public:
    DirtyLittleBassSynthAudioProcessorEditor(DirtyLittleBassSynthAudioProcessor &);
    ~DirtyLittleBassSynthAudioProcessorEditor();

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void timerCallback() override;

    DirtyLittleBassSynthAudioProcessor &processor;

    // Section divider rectangles, computed in resized() and drawn in paint().
    std::vector<juce::Rectangle<int>> dividers;

    OtherLookAndFeel dialLookAndFeel;
    OtherLookAndFeel dryWetLookAndFeel;

    GuiResources resources;

    TitleHeader titleHeader;
    TitleFooter titleFooter;

    OscPanel        oscPanel;
    AmpAdsrPanel    ampAdsrPanel;
    ModifierPanel   modifierPanel;
    FilterPanel     filterPanel;
    FilterAdsrPanel filterAdsrPanel;
    LfoPanel        lfoPanel;
    MasterPanel     masterPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirtyLittleBassSynthAudioProcessorEditor)
};
