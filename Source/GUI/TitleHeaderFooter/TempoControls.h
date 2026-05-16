/*
  ==============================================================================

    TempoControls.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "TempoSnapshot.h"

//==============================================================================

/// Centred BPM display in the title header. In standalone mode the user can
/// drag vertically (Shift = fine) or double-click to type a value, both writing
/// to the `tempo_fallback_bpm` APVTS param. In a host the value is read-only
/// and shows the host BPM with a "HOST" badge instead of "INT".
class TempoControls
    : public juce::Component
    , public juce::SettableTooltipClient
{
public:
    TempoControls(GuiResources &resources);
    ~TempoControls() override = default;

    void paint            (juce::Graphics &)         override;
    void resized          ()                         override;
    void mouseDown        (const juce::MouseEvent &) override;
    void mouseDrag        (const juce::MouseEvent &) override;
    void mouseUp          (const juce::MouseEvent &) override;
    void mouseDoubleClick (const juce::MouseEvent &) override;

    /// Pulls the latest tempo info from the snapshot and repaints if anything
    /// visible changed. Call from the editor's timer.
    void Update();

private:
    void CommitBpmEdit();
    void CancelBpmEdit();

    GuiResources &resources;

    TempoInfo currentTempo;

    // Inline text editor for typing a BPM value. Hidden until double-click.
    juce::TextEditor bpmEditor;

    // BPM drag-edit state. bpmDragging only ever set in standalone mode.
    bool  bpmDragging       = false;
    int   bpmDragStartY     = 0;
    float bpmDragStartValue = 120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoControls)
};
