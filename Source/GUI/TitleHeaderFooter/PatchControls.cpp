/*
  ==============================================================================

    PatchControls.cpp

  ==============================================================================
*/

#include "PatchControls.h"
#include "GuiHelpers.h"

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
    const auto &theme  = resources.theme;
    auto        bounds = getLocalBounds();

    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 14.0f
                                           , juce::Font::bold)));

    if (isDirty)
    {
        const auto dirtyArea = bounds.removeFromRight(15);
        g.setColour(theme.orangeAccent);
        g.drawText("*", dirtyArea, juce::Justification::centred);
    }

    const auto nameColour = (cachedSource == PatchManager::CurrentSource::Factory) ? theme.primaryAccent
                          : (cachedSource == PatchManager::CurrentSource::User)    ? theme.secondaryAccent
                          :                                                          theme.textPrimary;

    g.setColour(nameColour);

    // useEllipsesIfTooBig = true → name truncates with "…" when too long.
    g.drawText(curentPatchName, bounds, juce::Justification::centred, true);
}

void PatchNameDisplay::mouseDown(const juce::MouseEvent &e)
{
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

    const auto latestName   = resources.patchManager->GetCurrentPatchName();
    const bool latestDirty  = resources.patchManager->IsDirty();
    const auto latestSource = resources.patchManager->GetCurrentSource();

    if (latestName != curentPatchName || latestDirty != isDirty || latestSource != cachedSource)
    {
        curentPatchName = latestName;
        isDirty         = latestDirty;
        cachedSource    = latestSource;
        repaint();
    }
}

//==============================================================================

PatchControls::PatchControls(GuiResources &res)
: resources(res)
, nameDisplay(std::make_unique<PatchNameDisplay>(res))
, selectionPopup(res)
{
    setOpaque(false);
    
    const auto &theme = res.theme;

    nameDisplay->SetPatchName("Init");
    addAndMakeVisible(nameDisplay.get());

    nameDisplay->onClick = [this]()
    {
        selectionPopup.Show(nameDisplay.get());
    };
    
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
    
    deleteButton.setEnabled(false);

    initButton.onClick = [this]()
    {
        if (resources.patchManager != nullptr)
            resources.patchManager->LoadInit();
    };
    saveButton  .onClick = [this]() { TriggerSave();   };
    saveAsButton.onClick = [this]() { TriggerSaveAs(); };
    deleteButton.onClick = [this]()
    {
        ShowDeleteConfirmationDialog();
    };
    randomizeButton.onClick = [this]()
    {
        if (resources.patchManager != nullptr)
            resources.patchManager->RandomizeAll();
    };
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

    DLBS::SetTip(prevButton,      "Scroll to previous preset");
    DLBS::SetTip(nextButton,      "Scroll to next preset");
    DLBS::SetTip(initButton,      "Initialize patch");
    DLBS::SetTip(saveButton,      "Save current patch");
    DLBS::SetTip(saveAsButton,    "Save parameters as patch");
    DLBS::SetTip(deleteButton,    "Delete current patch");
    DLBS::SetTip(randomizeButton, "Randomize parameter values"); 
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

void PatchControls::paintOverChildren(juce::Graphics &g)
{
    if (! isFileDragOver)
        return;

    const auto &theme  = resources.theme;
    const auto  bounds = getLocalBounds().toFloat();

    g.setColour(theme.secondaryAccent.withAlpha(0.15f));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(theme.secondaryAccent.withAlpha(0.65f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.5f);
}

bool PatchControls::isInterestedInFileDrag(const juce::StringArray &files)
{
    for (const auto &path : files)
        if (path.endsWithIgnoreCase(".dlbs"))
            return true;

    return false;
}

void PatchControls::fileDragEnter(const juce::StringArray & /*files*/, int /*x*/, int /*y*/)
{
    if (! isFileDragOver)
    {
        isFileDragOver = true;
        repaint();
    }
}

void PatchControls::fileDragExit(const juce::StringArray & /*files*/)
{
    if (isFileDragOver)
    {
        isFileDragOver = false;
        repaint();
    }
}

void PatchControls::filesDropped(const juce::StringArray &files, int /*x*/, int /*y*/)
{
    if (isFileDragOver)
    {
        isFileDragOver = false;
        repaint();
    }

    if (resources.patchManager == nullptr)
        return;

    auto *pm = resources.patchManager;
    std::vector<juce::File> imported;

    for (const auto &path : files)
    {
        if (! path.endsWithIgnoreCase(".dlbs"))
            continue;

        if (auto destination = pm->ImportPatchFile(juce::File(path)))
            imported.push_back(*destination);
    }

    if (imported.empty())
        return;

    pm->RefreshPatchList();
    
    if (imported.size() == 1)
        pm->LoadPatch(imported.front());
}

void PatchControls::Update()
{
    if (nameDisplay != nullptr)
        nameDisplay->Update();

    if (resources.patchManager == nullptr)
        return;

    auto *pm = resources.patchManager;

    // Delete: only user patches are deletable.
    const bool deletable = pm->IsCurrentPatchUserOwned();
    if (deleteButton.isEnabled() != deletable)
        deleteButton.setEnabled(deletable);

    const bool hasPatches = ! pm->GetPatchList().empty();
    if (prevButton.isEnabled() != hasPatches) prevButton.setEnabled(hasPatches);
    if (nextButton.isEnabled() != hasPatches) nextButton.setEnabled(hasPatches);
}

void PatchControls::TriggerSave()
{
    if (resources.patchManager == nullptr)
        return;

    if (!resources.patchManager->SavePatch())
        ShowSaveAsDialog();
}

void PatchControls::TriggerSaveAs()
{
    ShowSaveAsDialog();
}

void PatchControls::ShowSaveAsDialog()
{
    if (resources.patchManager == nullptr)
        return;

    // shared_ptr keeps the AlertWindow alive until the callback fires.
    // Parenting to `this` lets JUCE dismiss it cleanly if PatchControls dies.
    auto window = std::make_shared<juce::AlertWindow>("Save Patch As"
                                                      , "Patch name:"
                                                      , juce::AlertWindow::NoIcon
                                                      , this);
    window->addTextEditor("name", resources.patchManager->GetCurrentPatchName(), {});
    window->addButton("Save"
                      , 1
                      , juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton("Cancel"
                      , 0
                      , juce::KeyPress(juce::KeyPress::escapeKey));
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

    if (!resources.patchManager->IsCurrentPatchUserOwned())
        return;

    const auto patchName = resources.patchManager->GetCurrentPatchName();
    const auto patchFile = resources.patchManager->GetCurrentPatchFile();

    auto window = std::make_shared<juce::AlertWindow>("Delete Patch?"
                                                      , "Are you sure you want to delete \""
                                                          + patchName
                                                          + "\"? This cannot be undone."
                                                      , juce::AlertWindow::NoIcon
                                                      , this);
    window->addButton("Cancel"
                      , 0
                      , juce::KeyPress(juce::KeyPress::escapeKey));
    window->addButton("Delete"
                      , 1
                      , juce::KeyPress(juce::KeyPress::returnKey));
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

    const auto clipboardText       = juce::SystemClipboard::getTextFromClipboard();
    const bool clipboardLooksValid = clipboardText.contains("<DLBSPatch");
    const bool canShowCurrentFile  = pm->IsCurrentPatchUserOwned();

    juce::PopupMenu menu;
    menu.addItem(Copy,            "Copy");
    menu.addItem(Paste,           "Paste",                    clipboardLooksValid);
    menu.addSeparator();
    menu.addItem(RevealFolder,    "Reveal Patches Folder");
    menu.addItem(ShowCurrentFile, "Show Current Patch File",  canShowCurrentFile);

    const auto options = juce::PopupMenu::Options()
                             .withTargetComponent(targetComponent)
                             .withTargetScreenArea(targetComponent->getScreenBounds());
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
