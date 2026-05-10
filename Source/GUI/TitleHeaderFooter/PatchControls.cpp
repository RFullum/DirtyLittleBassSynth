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

//==============================================================================

PatchControls::PatchControls(GuiResources &res)
: resources(res)
, nameDisplay(std::make_unique<PatchNameDisplay>(res))
{
    const auto &theme = res.theme;

//    patchNameButton.setButtonText("Init");
//    patchNameButton.setColour(juce::TextButton::buttonColourId,   juce::Colours::transparentBlack);
//    patchNameButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
//    patchNameButton.setColour(juce::TextButton::textColourOffId,  theme.textPrimary);
//    patchNameButton.setColour(juce::ComboBox::outlineColourId,    juce::Colours::transparentBlack);
//    addAndMakeVisible(patchNameButton);
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
