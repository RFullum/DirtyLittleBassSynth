/*
  ==============================================================================

    PatchControls.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//==============================================================================

/// Title-header patch cluster. Hosts every visible control for the patch
/// system: the clickable patch-name display, prev/next cycle arrows, and the
/// row of action buttons (Init / Save / Save As / Delete / Randomize).
///
/// This is a layout-only stage: the components are placed and styled but no
/// behavior is wired yet. Click handlers, popups, drag-and-drop, and the
/// dirty `*` indicator land in a follow-up pass.
class PatchControls
    : public juce::Component
{
public:
    PatchControls(GuiResources &resources);
    ~PatchControls() override = default;

    void resized() override;

private:
    GuiResources &resources;

    // Top row: patch-name display flanked by cycle arrows.
    juce::TextButton patchNameButton;   // styled as a label; clickable -> popup later
    juce::TextButton prevButton;
    juce::TextButton nextButton;

    // Bottom row: action buttons.
    juce::TextButton initButton;
    juce::TextButton saveButton;
    juce::TextButton saveAsButton;
    juce::TextButton deleteButton;
    juce::TextButton randomizeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchControls)
};
