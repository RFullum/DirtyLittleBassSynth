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
    addAndMakeVisible(tempoControls);
    addAndMakeVisible(midiLearnControls);
    addAndMakeVisible(patchControls);
}

void TitleHeader::paint(juce::Graphics &g)
{
    const auto &theme = resources.theme;

    // === Left: plugin name + tagline ===
    g.setColour(theme.primaryAccent);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 13.0f
                                           , juce::Font::bold))
              .withExtraKerningFactor(0.22f));
    g.drawText("DIRTY LITTLE BASS SYNTH"
               , pluginNameRect
               , juce::Justification::centredLeft);

    g.setColour(theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 9.0f
                                           , 0))
              .withExtraKerningFactor(0.12f));
    g.drawText(juce::String::fromUTF8("MONO \xc2\xb7 SINGLE VOICE")
               , taglineRect
               , juce::Justification::centredLeft);

    // === Right: brand text ===
    g.setColour(theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 11.0f
                                           , juce::Font::bold))
              .withExtraKerningFactor(0.18f));
    g.drawText("FULLUMMUSIC", brandingRect, juce::Justification::centredRight);
}

void TitleHeader::resized()
{
    constexpr int padding              = 14;
    constexpr int afterBranding        = 12;
    constexpr int afterPatch           = 10;
    constexpr int afterMidiLearn       = 6;
    constexpr int textPad              = 4;
    constexpr int bpmWidth             = 130;
    constexpr int bpmHeight            = 18;
    constexpr int patchWidth           = 18 + 4 + 18 + 10 + 64;   // arrow + gap + arrow + gap + INIT PATCH text
    constexpr int midiLearnWidth       = 56 + 6 + 88;             // LEARN + gap + CLEAR MAPS
    constexpr int controlsHeight       = 18;

    auto bounds = getLocalBounds();

    // Measure right-side branding width with its actual font so nothing clips.
    const auto brandingFont = juce::Font(juce::FontOptions("Helvetica"
                                                           , 11.0f
                                                           , juce::Font::bold))
                                        .withExtraKerningFactor(0.18f);
    const int brandingWidth = juce::GlyphArrangement::getStringWidthInt(brandingFont, "FULLUMMUSIC") + textPad;

    // Right: lay out from the right edge inward
    // (branding -> patch cluster -> midi learn cluster).
    int rightX     = bounds.getRight() - padding;
    int controlsY  = bounds.getY() + (bounds.getHeight() - controlsHeight) / 2;

    brandingRect = juce::Rectangle<int>(rightX - brandingWidth
                                        , bounds.getY() + (bounds.getHeight() - 16) / 2
                                        , brandingWidth
                                        , 16);
    rightX -= brandingWidth + afterBranding;

    patchControls.setBounds(rightX - patchWidth, controlsY, patchWidth, controlsHeight);
    rightX -= patchWidth + afterPatch;

    midiLearnControls.setBounds(rightX - midiLearnWidth, controlsY, midiLearnWidth, controlsHeight);
    rightX -= midiLearnWidth + afterMidiLearn;

    // Centre: BPM display, vertically centred in the header.
    tempoControls.setBounds((bounds.getWidth() - bpmWidth) / 2
                            , bounds.getY() + (bounds.getHeight() - bpmHeight) / 2
                            , bpmWidth
                            , bpmHeight);

    // Left: plugin name + tagline. Right edge is the left edge of the BPM area.
    int leftX     = bounds.getX() + padding;
    int leftRight = tempoControls.getX() - 8;

    pluginNameRect = juce::Rectangle<int>(leftX, bounds.getY() + 12, leftRight - leftX, 18);
    taglineRect    = juce::Rectangle<int>(leftX, bounds.getY() + 32, leftRight - leftX, 14);
}

void TitleHeader::Update()
{
    tempoControls    .Update();
    midiLearnControls.Update();
}
