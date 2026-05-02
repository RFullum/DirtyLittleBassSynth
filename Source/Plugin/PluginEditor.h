#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiResources.h"
#include "OtherLookAndFeel.h"
#include "TitleHeader.h"
#include "SourcesColumn.h"
#include "FilterColumn.h"
#include "ModifiersColumn.h"
#include "MasterColumn.h"

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirtyLittleBassSynthAudioProcessorEditor)
};
