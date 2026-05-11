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

    /// Pulls the current patch name and dirty flag from PatchManager and
    /// repaints only when something visible changed. Call from the editor's
    /// 60Hz timer via PatchControls::Update().
    void Update();

private:
    GuiResources &resources;
    juce::String  curentPatchName;
    bool          isDirty = false;
};

//==============================================================================

/// Title-header patch cluster. Hosts every visible control for the patch
/// system: the clickable patch-name display, prev/next cycle arrows, and the
/// row of action buttons (Init / Save / Save As / Delete / Randomize).
class PatchControls
    : public juce::Component
{
public:
    PatchControls(GuiResources &resources);
    ~PatchControls() override = default;

    void resized() override;

    /// Drives the timer-fed children (currently just the patch name display's
    /// dirty + name refresh). Call from the editor's 60Hz timer via
    /// TitleHeader::Update().
    void Update();

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
