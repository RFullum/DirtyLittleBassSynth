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
#include "TempoSnapshot.h"

//==============================================================================

/// Top-of-window header strip: plugin name + tagline on the left, BPM display
/// (centred), preset navigation placeholder + brand text on the right.
/// In standalone mode the BPM display is drag-editable and writes to the
/// `tempo_fallback_bpm` APVTS param. In a DAW it's a read-only label.
class TitleHeader
    : public juce::Component
{
public:
    TitleHeader(GuiResources &resources);
    ~TitleHeader() override = default;

    void paint            (juce::Graphics &)        override;
    void resized          ()                         override;
    void mouseDown        (const juce::MouseEvent &) override;
    void mouseDrag        (const juce::MouseEvent &) override;
    void mouseUp          (const juce::MouseEvent &) override;
    void mouseDoubleClick (const juce::MouseEvent &) override;

    /// Pulls the latest tempo info from the snapshot, repaints the BPM display
    /// if anything changed. Call from the editor's timer.
    void Update();

private:
    void CommitBpmEdit();
    void CancelBpmEdit();

    GuiResources &resources;

    TempoInfo currentTempo;

    // Inline text editor for typing a BPM value. Hidden until double-click.
    juce::TextEditor bpmEditor;

    juce::Rectangle<int> pluginNameRect;
    juce::Rectangle<int> taglineRect;
    juce::Rectangle<int> bpmRect;
    juce::Rectangle<int> initPatchRect;
    juce::Rectangle<int> prevBtnRect;
    juce::Rectangle<int> nextBtnRect;
    juce::Rectangle<int> brandingRect;

    // BPM drag-edit state. bpmDragging only ever set in standalone mode.
    bool  bpmDragging       = false;
    int   bpmDragStartY     = 0;
    float bpmDragStartValue = 120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleHeader)
};

//==============================================================================

/// Bottom-of-window footer strip: version on the left, URL on the right.
class TitleFooter
    : public juce::Component
{
public:
    TitleFooter();
    ~TitleFooter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void setTheme(const Palette::Theme &t);

private:
    const Palette::Theme *theme = nullptr;

    juce::Rectangle<int> versionRect;
    juce::Rectangle<int> urlRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleFooter)
};
