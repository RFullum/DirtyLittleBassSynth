/*
  ==============================================================================

    PatchControls.cpp

  ==============================================================================
*/

#include "PatchControls.h"

//==============================================================================

PatchNameDisplay::PatchNameDisplay(GuiResources &res)
: juce::Button("")
, resources(res)
{
    setOpaque(false);
}

PatchNameDisplay::~PatchNameDisplay() {}

void PatchNameDisplay::paintButton(juce::Graphics &g, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
{
    const auto &theme = resources.theme;
    auto bounds = getLocalBounds();
    g.setColour(theme.textPrimary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 14.0f
                                           , juce::Font::bold)));
              
    if (isDirty)
    {
        auto dirtyArea = bounds.removeFromRight(15);
        g.drawText("*", dirtyArea, juce::Justification::centred);
    }
    
    g.drawText(curentPatchName, bounds, juce::Justification::centred);
}

void PatchNameDisplay::mouseDown(const juce::MouseEvent &e)
{
    // isPopupMenu unifies right-click (Win/Linux) and cmd/ctrl-click (macOS)
    // into a single "user wants the alt menu" signal. Anything else falls
    // through to juce::Button's default mouseDown so left-clicks still
    // trigger the normal onClick path.
    if (e.mods.isPopupMenu())
    {
        if (onAltClick != nullptr)
            onAltClick();

        return;
    }

    juce::Button::mouseDown(e);
}

void PatchNameDisplay::SetPatchName(juce::StringRef name)
{
    curentPatchName = name;
    repaint();
}

void PatchNameDisplay::Update()
{
    if (resources.patchManager == nullptr)
        return;

    // Pull both fields together so we don't repaint twice when both changed
    // (e.g. loading a different patch flips dirty false and name simultaneously).
    const auto latestName  = resources.patchManager->GetCurrentPatchName();
    const bool latestDirty = resources.patchManager->IsDirty();

    if (latestName != curentPatchName || latestDirty != isDirty)
    {
        curentPatchName = latestName;
        isDirty         = latestDirty;
        repaint();
    }
}

//==============================================================================

PatchControls::PatchControls(GuiResources &res)
: resources(res)
, nameDisplay(std::make_unique<PatchNameDisplay>(res))
, selectionPopup(res)
{
    const auto &theme = res.theme;

    nameDisplay->SetPatchName("Init");
    addAndMakeVisible(nameDisplay.get());

    // Left-click on the patch name opens the selection popup.
    nameDisplay->onClick = [this]()
    {
        selectionPopup.Show(nameDisplay.get());
    };

    // Right-click (or cmd/ctrl-click) opens the alt-popup: Copy / Paste /
    // Reveal Folder / Show Current Patch File.
    nameDisplay->onAltClick = [this]()
    {
        ShowAltPopupMenu(nameDisplay.get());
    };

    prevButton.setButtonText("<");
    prevButton.setColour(juce::TextButton::buttonColourId,  theme.structure);
    prevButton.setColour(juce::TextButton::textColourOffId, theme.textSecondary);
    addAndMakeVisible(prevButton);

    nextButton.setButtonText(">");
    nextButton.setColour(juce::TextButton::buttonColourId,  theme.structure);
    nextButton.setColour(juce::TextButton::textColourOffId, theme.textSecondary);
    addAndMakeVisible(nextButton);

    auto styleActionButton = [&theme] (juce::TextButton &b, const juce::String &label)
    {
        b.setButtonText(label);
        b.setColour(juce::TextButton::buttonColourId,  theme.structure);
        b.setColour(juce::TextButton::textColourOffId, theme.textSecondary);
    };

    styleActionButton(initButton,      "INIT");
    styleActionButton(saveButton,      "SAVE");
    styleActionButton(saveAsButton,    "SAVE AS");
    styleActionButton(deleteButton,    "DELETE");
    styleActionButton(randomizeButton, "RANDOM");

    addAndMakeVisible(initButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(saveAsButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(randomizeButton);

    // Initial Delete state: we start on Init, which isn't deletable. Update()
    // syncs this on every timer tick from IsCurrentPatchUserOwned(); setting
    // it here avoids a one-tick flicker on first launch.
    deleteButton.setEnabled(false);

    // === Click handlers ===

    // INIT: drop the current state back to defaults. Patch name display will
    // start reading "Init" from PatchManager once Update() wiring lands in B5.
    initButton.onClick = [this]()
    {
        if (resources.patchManager != nullptr)
            resources.patchManager->LoadInit();
    };

    // SAVE: overwrite the current user patch in place. On Init/Factory state
    // SavePatch() returns false and we fall through to the Save As dialog so
    // the user is never left wondering why SAVE did nothing.
    saveButton.onClick = [this]()
    {
        if (resources.patchManager == nullptr)
            return;

        if (! resources.patchManager->SavePatch())
            ShowSaveAsDialog();
    };

    // SAVE AS: always opens the dialog, regardless of current source. Pre-fill
    // and auto-increment behaviour live inside ShowSaveAsDialog / SavePatchAs.
    saveAsButton.onClick = [this]()
    {
        ShowSaveAsDialog();
    };

    // DELETE: confirm-then-delete. The button itself is disabled when the
    // current patch isn't user-owned, so by the time we get here the
    // confirmation dialog should always have a real file to act on.
    deleteButton.onClick = [this]()
    {
        ShowDeleteConfirmationDialog();
    };

    // RANDOMIZE: roll all sound-design params and force the master safety
    // params to known-safe values. Leaves the user on the same patch name
    // with unsaved changes (dirty *).
    randomizeButton.onClick = [this]()
    {
        if (resources.patchManager != nullptr)
            resources.patchManager->RandomizeAll();
    };

    // PREV / NEXT: cycle through the sorted patch list, wrapping at the ends.
    // From Init, prev lands on the last patch; next lands on the first.
    prevButton.onClick = [this]()
    {
        if (resources.patchManager != nullptr)
            resources.patchManager->StepPatch(-1);
    };

    nextButton.onClick = [this]()
    {
        if (resources.patchManager != nullptr)
            resources.patchManager->StepPatch(+1);
    };
}

void PatchControls::Update()
{
    if (nameDisplay != nullptr)
        nameDisplay->Update();

    // Sync the Delete button's enabled state with PatchManager — only user
    // patches are deletable. Factory and Init disable + dim Delete via JUCE's
    // default disabled appearance.
    if (resources.patchManager != nullptr)
    {
        const bool deletable = resources.patchManager->IsCurrentPatchUserOwned();

        if (deleteButton.isEnabled() != deletable)
            deleteButton.setEnabled(deletable);
    }
}

void PatchControls::ShowSaveAsDialog()
{
    if (resources.patchManager == nullptr)
        return;

    // shared_ptr keeps the AlertWindow alive until the modal callback fires.
    // Parenting to `this` lets JUCE dismiss it cleanly if PatchControls dies.
    auto window = std::make_shared<juce::AlertWindow>("Save Patch As"
                                                      , "Patch name:"
                                                      , juce::AlertWindow::NoIcon
                                                      , this);

    // Pre-fill with the current name. From a User patch this lets the user
    // duplicate by editing one character; from Init/Factory it gives them a
    // sensible starting point (e.g. SAVE-from-Factory means "make my own copy
    // of this factory preset"). SavePatchAs auto-increments on collision so
    // accepting the prefilled name is always safe — never overwrites.
    window->addTextEditor("name", resources.patchManager->GetCurrentPatchName(), {});

    window->addButton("Save"
                      , 1
                      , juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton("Cancel"
                      , 0
                      , juce::KeyPress(juce::KeyPress::escapeKey));

    // Capture the PatchManager pointer + window by value rather than `this`
    // so the callback is insulated from PatchControls being destroyed during
    // the modal (window itself is parented to `this`, so it would be dismissed
    // first anyway — the capture is defensive).
    window->enterModalState(true
                            , juce::ModalCallbackFunction::create(
                                [pm = resources.patchManager, window] (int result)
                                {
                                    if (result == 1)
                                    {
                                        const auto typed = window->getTextEditorContents("name");
                                        pm->SavePatchAs(typed);
                                    }
                                })
                            , false);
}

void PatchControls::ShowDeleteConfirmationDialog()
{
    if (resources.patchManager == nullptr)
        return;

    // Defensive: the button should be disabled when there's nothing to delete,
    // but never trust the GUI gate alone. Refusing here also handles the
    // pathological case where the current source changed between the click
    // and this method running (e.g. via a future keyboard shortcut).
    if (! resources.patchManager->IsCurrentPatchUserOwned())
        return;

    const auto patchName = resources.patchManager->GetCurrentPatchName();
    const auto patchFile = resources.patchManager->GetCurrentPatchFile();

    auto window = std::make_shared<juce::AlertWindow>("Delete Patch?"
                                                      , "Are you sure you want to delete \""
                                                          + patchName
                                                          + "\"? This cannot be undone."
                                                      , juce::AlertWindow::QuestionIcon
                                                      , this);

    window->addButton("Cancel"
                      , 0
                      , juce::KeyPress(juce::KeyPress::escapeKey));
    window->addButton("Delete"
                      , 1
                      , juce::KeyPress(juce::KeyPress::returnKey));

    // Capture the file by value at dialog-open time so the callback acts on
    // the patch the user was looking at when they confirmed, not whatever's
    // current after the modal closes. (Modal blocks input so the current patch
    // can't actually change during the dialog, but the explicit capture makes
    // the contract obvious.)
    window->enterModalState(true
                            , juce::ModalCallbackFunction::create(
                                [pm = resources.patchManager, patchFile, window] (int result)
                                {
                                    if (result == 1)
                                        pm->DeletePatch(patchFile);
                                })
                            , false);
}

void PatchControls::ShowAltPopupMenu(juce::Component *targetComponent)
{
    if (resources.patchManager == nullptr || targetComponent == nullptr)
        return;

    auto *pm = resources.patchManager;

    enum AltMenuId : int
    {
        Copy = 1,
        Paste,
        RevealFolder,
        ShowCurrentFile
    };

    // Cheap clipboard pre-check: does the clipboard text plausibly contain a
    // DLBSPatch root? Full validation runs inside ApplyFromClipboard if the
    // user picks Paste, so this is just a UX gate — we want Paste to look
    // disabled when there's clearly nothing to paste.
    const auto clipboardText      = juce::SystemClipboard::getTextFromClipboard();
    const bool clipboardLooksValid = clipboardText.contains("<DLBSPatch");

    // Show Current Patch File only makes sense for user patches: factory
    // files live inside the bundle (revealing them dumps Finder into the
    // app's Contents/Resources/), Init has no file at all.
    const bool canShowCurrentFile = pm->IsCurrentPatchUserOwned();

    juce::PopupMenu menu;
    menu.addItem(Copy,            "Copy");
    menu.addItem(Paste,           "Paste",                    clipboardLooksValid);
    menu.addSeparator();
    menu.addItem(RevealFolder,    "Reveal Patches Folder");
    menu.addItem(ShowCurrentFile, "Show Current Patch File",  canShowCurrentFile);

    const auto options = juce::PopupMenu::Options()
                             .withTargetComponent(targetComponent)
                             .withTargetScreenArea(targetComponent->getScreenBounds());

    // Capture file paths by value at menu-open time so the callback isn't
    // sensitive to state changes between showing the menu and the user
    // selecting an item (none should happen in practice, but the snapshots
    // are free).
    const auto currentFile = pm->GetCurrentPatchFile();
    const auto userDir     = pm->GetUserPatchesDirectory();

    menu.showMenuAsync(options, [pm, currentFile, userDir] (int chosen)
    {
        switch (chosen)
        {
            case Copy:            pm->SerializeToClipboard();   break;
            case Paste:           pm->ApplyFromClipboard();     break;
            case RevealFolder:    userDir.revealToUser();       break;
            case ShowCurrentFile: currentFile.revealToUser();   break;
            default:                                            break;   // 0 = dismissed
        }
    });
}

void PatchControls::resized()
{
    static constexpr int rowGap     = 4;
    static constexpr int btnGap     = 4;
    static constexpr int arrowWidth = 28;

    auto bounds = getLocalBounds();
    const int rowHeight = (bounds.getHeight() - rowGap) / 2;

    auto topRow    = bounds.removeFromTop(rowHeight);
    bounds.removeFromTop(rowGap);
    auto bottomRow = bounds.removeFromTop(rowHeight);

    prevButton     .setBounds(topRow.removeFromLeft(arrowWidth));
    topRow         .removeFromLeft(btnGap);
    nextButton     .setBounds(topRow.removeFromRight(arrowWidth));
    topRow         .removeFromRight(btnGap);
    nameDisplay   ->setBounds(topRow);

    static constexpr int numButtons     = 5;
    static constexpr int totalGapsWidth = btnGap * (numButtons - 1);

    const int actionBtnWidth = (bottomRow.getWidth() - totalGapsWidth) / numButtons;

    initButton     .setBounds(bottomRow.removeFromLeft(actionBtnWidth));
    bottomRow      .removeFromLeft(btnGap);
    saveButton     .setBounds(bottomRow.removeFromLeft(actionBtnWidth));
    bottomRow      .removeFromLeft(btnGap);
    saveAsButton   .setBounds(bottomRow.removeFromLeft(actionBtnWidth));
    bottomRow      .removeFromLeft(btnGap);
    deleteButton   .setBounds(bottomRow.removeFromLeft(actionBtnWidth));
    bottomRow      .removeFromLeft(btnGap);
    randomizeButton.setBounds(bottomRow);
}
