/*
  ==============================================================================

    PatchControls.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//==============================================================================

class PatchNameDisplay
    : public juce::Button
{
public:
    PatchNameDisplay(GuiResources &res);
    ~PatchNameDisplay();
    
    void paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    void SetPatchName(juce::StringRef name);
    
private:
    GuiResources &resources;
    juce::String  curentPatchName;
    bool          isDirty = false;
};

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
    
//    juce::TextButton patchNameButton;   // styled as a label; clickable -> popup later
    std::unique_ptr<PatchNameDisplay> nameDisplay;
    juce::TextButton                  prevButton;
    juce::TextButton                  nextButton;

    juce::TextButton initButton;
    juce::TextButton saveButton;
    juce::TextButton saveAsButton;
    juce::TextButton deleteButton;
    juce::TextButton randomizeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchControls)
};
