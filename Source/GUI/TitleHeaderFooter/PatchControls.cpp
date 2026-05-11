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
{
    const auto &theme = res.theme;

    nameDisplay->SetPatchName("Init");
    addAndMakeVisible(nameDisplay.get());

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

    // === Click handlers ===

    // INIT: drop the current state back to defaults. Patch name display will
    // start reading "Init" from PatchManager once Update() wiring lands in B5.
    initButton.onClick = [this]()
    {
        if (resources.patchManager != nullptr)
            resources.patchManager->LoadInit();
    };

    // SAVE: overwrite the current user patch in place. On Init/Factory state,
    // SavePatch() returns false and the GUI is expected to open the Save As
    // dialog — that dialog wiring lands in C2. Until then the fall-through is
    // a silent no-op.
    saveButton.onClick = [this]()
    {
        if (resources.patchManager == nullptr)
            return;

        if (! resources.patchManager->SavePatch())
        {
            // TODO (C2): open Save As dialog here when current is Init/Factory.
        }
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
