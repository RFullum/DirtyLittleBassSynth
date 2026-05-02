/*
  ==============================================================================

    TitleHeader.cpp
    Created: 12 Jan 2021 11:10:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TitleHeader.h"

//==============================================================================

TitleHeader::TitleHeader() {}

void TitleHeader::paint(juce::Graphics &g)
{
    if (theme == nullptr)
        return;

    // === Left: plugin name + tagline ===
    g.setColour(theme->primaryAccent);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 13.0f, juce::Font::bold))
                 .withExtraKerningFactor(0.22f));
    g.drawText("DIRTY LITTLE BASS SYNTH", pluginNameRect, juce::Justification::centredLeft);

    g.setColour(theme->textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                 .withExtraKerningFactor(0.12f));
    g.drawText(juce::String::fromUTF8("MONO \xc2\xb7 SINGLE VOICE"),
               taglineRect, juce::Justification::centredLeft);

    // === Right: "Init Patch" label ===
    g.setColour(theme->textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                 .withExtraKerningFactor(0.10f));
    g.drawText("INIT PATCH", initPatchRect, juce::Justification::centredRight);

    // === Right: prev / next preset placeholder buttons ===
    auto drawArrowBtn = [&](juce::Rectangle<int> r, bool pointsLeft)
    {
        g.setColour(theme->structure);
        g.fillRoundedRectangle(r.toFloat(), 3.0f);

        g.setColour(theme->textSecondary);
        g.drawRoundedRectangle(r.toFloat().reduced(0.5f), 3.0f, 1.0f);

        const float cx = (float)r.getCentreX();
        const float cy = (float)r.getCentreY();
        const float w  = 4.0f;
        const float h  = 6.0f;

        juce::Path arrow;
        if (pointsLeft)
            arrow.addTriangle(cx + w * 0.5f, cy - h * 0.5f,
                              cx + w * 0.5f, cy + h * 0.5f,
                              cx - w * 0.5f, cy);
        else
            arrow.addTriangle(cx - w * 0.5f, cy - h * 0.5f,
                              cx - w * 0.5f, cy + h * 0.5f,
                              cx + w * 0.5f, cy);

        g.setColour(theme->textSecondary);
        g.fillPath(arrow);
    };

    drawArrowBtn(prevBtnRect, /*pointsLeft*/ true);
    drawArrowBtn(nextBtnRect, /*pointsLeft*/ false);

    // === Right: brand text ===
    g.setColour(theme->textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 11.0f, juce::Font::bold))
                 .withExtraKerningFactor(0.18f));
    g.drawText("FULLUMMUSIC", brandingRect, juce::Justification::centredRight);
}

void TitleHeader::resized()
{
    constexpr int padding   = 14;
    constexpr int btnSize   = 18;
    constexpr int btnGap    = 4;
    constexpr int afterBtns = 12;
    constexpr int initGap   = 10;
    constexpr int textPad   = 4;

    auto bounds = getLocalBounds();

    // Measure right-side text widths using their actual fonts so nothing clips.
    const auto brandingFont = juce::Font(juce::FontOptions("Helvetica", 11.0f, juce::Font::bold))
                                  .withExtraKerningFactor(0.18f);
    const auto initFont     = juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                                  .withExtraKerningFactor(0.10f);

    const int brandingWidth = juce::GlyphArrangement::getStringWidthInt(brandingFont, "FULLUMMUSIC") + textPad;
    const int initWidth     = juce::GlyphArrangement::getStringWidthInt(initFont,     "INIT PATCH")  + textPad;

    // Right: lay out from the right edge inward (branding -> next -> prev -> init label).
    int rightX = bounds.getRight() - padding;
    int btnY   = bounds.getY() + (bounds.getHeight() - btnSize) / 2;

    brandingRect = juce::Rectangle<int>(rightX - brandingWidth,
                                        bounds.getY() + (bounds.getHeight() - 16) / 2,
                                        brandingWidth, 16);
    rightX -= brandingWidth + afterBtns;

    nextBtnRect = juce::Rectangle<int>(rightX - btnSize, btnY, btnSize, btnSize);
    rightX -= btnSize + btnGap;

    prevBtnRect = juce::Rectangle<int>(rightX - btnSize, btnY, btnSize, btnSize);
    rightX -= btnSize + initGap;

    initPatchRect = juce::Rectangle<int>(rightX - initWidth,
                                         bounds.getY() + (bounds.getHeight() - 14) / 2,
                                         initWidth, 14);

    // Left: plugin name + tagline. Right edge is the left edge of the right-side cluster.
    int leftX     = bounds.getX() + padding;
    int leftRight = initPatchRect.getX() - 8;

    pluginNameRect = juce::Rectangle<int>(leftX, bounds.getY() + 12, leftRight - leftX, 18);
    taglineRect    = juce::Rectangle<int>(leftX, bounds.getY() + 32, leftRight - leftX, 14);
}

void TitleHeader::SetTheme(const Palette::Theme &t)
{
    theme = &t;
    repaint();
}

//==============================================================================

TitleFooter::TitleFooter() {}

void TitleFooter::paint(juce::Graphics &g)
{
    if (theme == nullptr)
        return;

    g.setColour(theme->textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                 .withExtraKerningFactor(0.08f));
    g.drawText("VERSION 1.1.2", versionRect, juce::Justification::centredLeft);

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
