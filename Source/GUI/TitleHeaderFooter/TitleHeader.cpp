/*
  ==============================================================================

    TitleHeader.cpp
    Created: 12 Jan 2021 11:10:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TitleHeader.h"
#include "GuiHelpers.h"

//==============================================================================

TitleHeader::TitleHeader(GuiResources &res)
: resources(res)
, tempoControls(res)
, midiLearnControls(res)
, patchControls(res)
{
    setOpaque(false);
    addAndMakeVisible(tempoControls);
    addAndMakeVisible(midiLearnControls);
    addAndMakeVisible(patchControls);

    panicButton.setButtonText      ("PANIC");
    panicButton.setColour          (juce::TextButton::buttonColourId,  res.theme.structure);
    panicButton.setColour          (juce::TextButton::textColourOffId, res.theme.orangeAccent);
    panicButton.onClick = [this]()
    {
        if (resources.midiPanic)
            resources.midiPanic();
    };
    addAndMakeVisible(panicButton);
    DLBS::SetTip(panicButton, "MIDI Panic: Click to kill stuck MIDI");

    // === Plugin-name label ===
    // Painted style mirrors what the old paint() routine drew: primaryAccent,
    // Helvetica 20 bold, kerned, bottom-left justified. The Label gives us a
    // place to attach the right-click handler + a hover tooltip pointing users
    // at the hidden menu.
    pluginNameLabel.setText("DIRTY LITTLE BASS SYNTH", juce::dontSendNotification);
    pluginNameLabel.setColour       (juce::Label::textColourId, res.theme.primaryAccent);
    pluginNameLabel.setFont         (juce::Font(juce::FontOptions("Helvetica"
                                                                   , 20.0f
                                                                   , juce::Font::bold))
                                     .withExtraKerningFactor(0.22f));
    pluginNameLabel.setJustificationType(juce::Justification::bottomLeft);
    pluginNameLabel.setBorderSize(juce::BorderSize<int>(0));
    pluginNameLabel.setInterceptsMouseClicks(true, false);
    pluginNameLabel.setTooltip("Right-click for menu");
    addAndMakeVisible(pluginNameLabel);
}

void TitleHeader::TitleLabel::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isPopupMenu())
    {
        header.ShowAppMenu();
        return;
    }

    juce::Label::mouseDown(e);
}

void TitleHeader::ShowAppMenu()
{
    if (! resources.getTooltipsEnabled || ! resources.setTooltipsEnabled)
        return;

    const bool tipsOn = resources.getTooltipsEnabled();

    juce::PopupMenu menu;
    juce::PopupMenu::Item tooltipItem;
    tooltipItem.itemID         = 1;
    tooltipItem.text           = "Show Tooltips";
    tooltipItem.shortcutKeyDescription = "F1";
    tooltipItem.isTicked       = tipsOn;
    tooltipItem.action         = [this, tipsOn]()
    {
        if (resources.setTooltipsEnabled)
            resources.setTooltipsEnabled(! tipsOn);
    };
    menu.addItem(tooltipItem);

    menu.showMenuAsync(juce::PopupMenu::Options()
                       .withTargetComponent(&pluginNameLabel));
}

void TitleHeader::paint(juce::Graphics &g)
{
    const auto &theme = resources.theme;

    // === Left: tagline (plugin name is a child Label, see ctor + resized) ===
    g.setColour(theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 10.0f
                                           , 0))
              .withExtraKerningFactor(0.12f));
    g.drawText(juce::String::fromUTF8("MONO BASS")
               , taglineRect
               , juce::Justification::topLeft);

    // === Right: brand text ===
    g.setColour(theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 13.0f
                                           , juce::Font::bold))
              .withExtraKerningFactor(0.18f));
    g.drawText("FULLUMMUSIC", brandingRect, juce::Justification::centredRight);
}

void TitleHeader::resized()
{
    static constexpr int sectionGap = 20;
    
    auto bounds = getLocalBounds();
    bounds.reduce(5, 0);
    
    taglineRect    = bounds.removeFromLeft(375);
    pluginNameRect = taglineRect.removeFromTop(taglineRect.proportionOfHeight(0.66f));
    pluginNameLabel.setBounds(pluginNameRect);

    auto rightCluster = bounds.removeFromRight(125);
    brandingRect      = rightCluster.removeFromTop(rightCluster.proportionOfHeight(0.6f));
    rightCluster.removeFromBottom(6);
    panicButton.setBounds(rightCluster.reduced(20, 0));

    bounds.removeFromLeft(sectionGap);
    tempoControls.setBounds(bounds.removeFromLeft(150));
    
    bounds.removeFromLeft(sectionGap);
    midiLearnControls.setBounds(bounds.removeFromLeft(200).reduced(0, 20));
    
    bounds.removeFromLeft(sectionGap);
    bounds.removeFromRight(sectionGap);
    patchControls.setBounds(bounds.reduced(0, 5));
    
}

void TitleHeader::Update()
{
    tempoControls    .Update();
    midiLearnControls.Update();
    patchControls    .Update();
}

void TitleHeader::TriggerPatchSave()    { patchControls.TriggerSave();   }
void TitleHeader::TriggerPatchSaveAs()  { patchControls.TriggerSaveAs(); }
