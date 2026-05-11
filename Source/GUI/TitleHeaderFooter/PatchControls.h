/*
  ==============================================================================

    PatchControls.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "PatchSelectionPopup.h"

//==============================================================================

class PatchNameDisplay
    : public juce::Button
{
public:
    PatchNameDisplay(GuiResources &res);
    ~PatchNameDisplay();

    void paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void mouseDown   (const juce::MouseEvent &e) override;

    void SetPatchName(juce::StringRef name);

    /// Pulls the current patch name and dirty flag from PatchManager and
    /// repaints only when something visible changed. Call from the editor's
    /// 60Hz timer via PatchControls::Update().
    void Update();

    /// Fired on right-click (and cmd-click on macOS, ctrl-click on
    /// Win/Linux — anything `MouseEvent::mods::isPopupMenu()` matches).
    /// Mirrors juce::Button::onClick for left-clicks. Empty by default.
    std::function<void()> onAltClick;

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
    /// Opens the async modal Save As dialog: text-input AlertWindow pre-filled
    /// with the current patch name. Submitting hands the typed string to
    /// PatchManager::SavePatchAs, which sanitises and auto-increments. Used
    /// by both the SAVE AS button and the SAVE button's fall-through path
    /// when the current state is Init or Factory.
    void ShowSaveAsDialog();

    /// Opens the async modal "Are you sure?" prompt before deleting the
    /// currently-loaded user patch. Confirm hands the file to
    /// PatchManager::DeletePatch, which drops the live state to Init.
    /// No-op if the current patch isn't user-owned (button should be
    /// disabled in that case, but the guard is defensive).
    void ShowDeleteConfirmationDialog();

    /// Opens the async right-click alt-popup anchored to `targetComponent`'s
    /// screen area. Items: Copy / Paste / Reveal Patches Folder / Show
    /// Current Patch File. Paste is disabled when the clipboard doesn't
    /// contain a plausible DLBSPatch; Show Current Patch File is disabled
    /// when the current state is Init or Factory.
    void ShowAltPopupMenu(juce::Component *targetComponent);

    GuiResources &resources;
    
//    juce::TextButton patchNameButton;   // styled as a label; clickable -> popup later
    std::unique_ptr<PatchNameDisplay> nameDisplay;
    PatchSelectionPopup               selectionPopup;
    juce::TextButton                  prevButton;
    juce::TextButton                  nextButton;

    juce::TextButton initButton;
    juce::TextButton saveButton;
    juce::TextButton saveAsButton;
    juce::TextButton deleteButton;
    juce::TextButton randomizeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchControls)
};
