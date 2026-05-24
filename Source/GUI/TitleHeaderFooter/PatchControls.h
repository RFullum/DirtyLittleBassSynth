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
    void Update();

    std::function<void()> onAltClick;

private:
    GuiResources &resources;
    juce::String  curentPatchName;
    bool          isDirty = false;

    PatchManager::CurrentSource cachedSource { PatchManager::CurrentSource::Init };
};

//==============================================================================

class PatchControls
    : public juce::Component
    , public juce::FileDragAndDropTarget
{
public:
    PatchControls(GuiResources &resources);
    ~PatchControls() override = default;

    void resized           () override;
    void paintOverChildren (juce::Graphics &) override;

    bool isInterestedInFileDrag (const juce::StringArray &files) override;
    void fileDragEnter          (const juce::StringArray &files, int x, int y) override;
    void fileDragExit           (const juce::StringArray &files) override;
    void filesDropped           (const juce::StringArray &files, int x, int y) override;

    void Update();
    void TriggerSave();
    void TriggerSaveAs();

private:
    void ShowSaveAsDialog();
    void ShowDeleteConfirmationDialog();
    void ShowAltPopupMenu(juce::Component *targetComponent);

    GuiResources &resources;
    
    std::unique_ptr<PatchNameDisplay> nameDisplay;
    PatchSelectionPopup               selectionPopup;
    juce::TextButton                  prevButton;
    juce::TextButton                  nextButton;

    juce::TextButton initButton;
    juce::TextButton saveButton;
    juce::TextButton saveAsButton;
    juce::TextButton deleteButton;
    juce::TextButton randomizeButton;

    /// True while a `.dlbs` drag is hovering over us. paintOverChildren reads
    /// this to tint the cluster.
    bool isFileDragOver = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchControls)
};
