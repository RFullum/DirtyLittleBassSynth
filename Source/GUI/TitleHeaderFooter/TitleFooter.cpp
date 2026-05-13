/*
  ==============================================================================

    TitleFooter.cpp
    Created: 10 May 2026 11:35:13am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TitleFooter.h"

//==============================================================================

TitleFooter::TitleFooter()
{
    setOpaque(false);
}

void TitleFooter::paint(juce::Graphics &g)
{
    if (theme == nullptr)
        return;

    g.setColour(theme->textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                 .withExtraKerningFactor(0.08f));
    g.drawText("VERSION 2.0.0", versionRect, juce::Justification::centredLeft);

    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                 .withExtraKerningFactor(0.06f));
    g.drawText("FULLUMMUSIC.COM", urlRect, juce::Justification::centredRight);
}

void TitleFooter::resized()
{
    constexpr int padding = 14;
    auto bounds = getLocalBounds();

    versionRect = bounds.reduced(padding, 0).withWidth(120);
    urlRect     = bounds.reduced(padding, 0).withTrimmedLeft(bounds.getWidth() - 200);
}

void TitleFooter::setTheme(const Palette::Theme &t)
{
    theme = &t;
    repaint();
}
