/*
  ==============================================================================

    TitleHeader.cpp
    Created: 12 Jan 2021 11:10:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TitleHeader.h"

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
}

void TitleHeader::paint(juce::Graphics &g)
{
    const auto &theme = resources.theme;

    // === Left: plugin name + tagline ===
    g.setColour(theme.primaryAccent);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 20.0f
                                           , juce::Font::bold))
              .withExtraKerningFactor(0.22f));
    g.drawText("DIRTY LITTLE BASS SYNTH"
               , pluginNameRect
               , juce::Justification::bottomLeft);

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
