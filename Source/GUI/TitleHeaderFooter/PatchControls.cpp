/*
  ==============================================================================

    PatchControls.cpp

  ==============================================================================
*/

#include "PatchControls.h"

//==============================================================================

PatchControls::PatchControls(GuiResources &res)
: resources(res)
{
}

void PatchControls::paint(juce::Graphics &g)
{
    const auto &theme = resources.theme;

    // === "Init Patch" label ===
    g.setColour(theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0)).withExtraKerningFactor(0.10f));
    g.drawText("INIT PATCH", initPatchRect, juce::Justification::centredRight);

    // === prev / next preset placeholder buttons ===
    auto drawArrowBtn = [&](juce::Rectangle<int> r, bool pointsLeft)
    {
        g.setColour(theme.structure);
        g.fillRoundedRectangle(r.toFloat(), 3.0f);

        g.setColour(theme.textSecondary);
        g.drawRoundedRectangle(r.toFloat().reduced(0.5f), 3.0f, 1.0f);

        const float cx = (float)r.getCentreX();
        const float cy = (float)r.getCentreY();
        const float w  = 4.0f;
        const float h  = 6.0f;

        juce::Path arrow;
        if (pointsLeft)
            arrow.addTriangle(cx + w * 0.5f
                              , cy - h * 0.5f
                              , cx + w * 0.5f
                              , cy + h * 0.5f
                              , cx - w * 0.5f
                              , cy);
        else
            arrow.addTriangle(cx - w * 0.5f
                              , cy - h * 0.5f
                              , cx - w * 0.5f
                              , cy + h * 0.5f
                              , cx + w * 0.5f
                              , cy);

        g.setColour(theme.textSecondary);
        g.fillPath(arrow);
    };

    drawArrowBtn(prevBtnRect, /*pointsLeft*/ true);
    drawArrowBtn(nextBtnRect, /*pointsLeft*/ false);
}

void PatchControls::resized()
{
    // Layout matches the original TitleHeader right-to-left placement:
    // next arrow, prev arrow, INIT PATCH label, all vertically centred.
    constexpr int btnSize  = 18;
    constexpr int btnGap   = 4;
    constexpr int initGap  = 10;
    constexpr int textPad  = 4;

    const auto initFont = juce::Font(juce::FontOptions("Helvetica", 9.0f, 0)).withExtraKerningFactor(0.10f);
    const int initWidth = juce::GlyphArrangement::getStringWidthInt(initFont, "INIT PATCH") + textPad;

    auto bounds = getLocalBounds();

    int rightX = bounds.getRight();
    int btnY   = bounds.getY() + (bounds.getHeight() - btnSize) / 2;

    nextBtnRect = juce::Rectangle<int>(rightX - btnSize, btnY, btnSize, btnSize);
    rightX -= btnSize + btnGap;

    prevBtnRect = juce::Rectangle<int>(rightX - btnSize, btnY, btnSize, btnSize);
    rightX -= btnSize + initGap;

    initPatchRect = juce::Rectangle<int>(rightX - initWidth
                                         , bounds.getY() + (bounds.getHeight() - 14) / 2
                                         , initWidth
                                         , 14);
}
