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
{
    // Inline BPM text editor: hidden by default; appears on double-click in
    // standalone mode for direct typing of the tempo value.
    const auto editorFont = juce::Font(juce::FontOptions("Helvetica", 12.0f, juce::Font::bold))
                                .withExtraKerningFactor(0.10f);

    bpmEditor.setMultiLine                   (false);
    bpmEditor.setReturnKeyStartsNewLine      (false);
    bpmEditor.setEscapeAndReturnKeysConsumed (true);
    bpmEditor.setSelectAllWhenFocused        (true);
    bpmEditor.setInputRestrictions           (8, "0123456789.");
    bpmEditor.setBorder                      (juce::BorderSize<int>(0));
    bpmEditor.setJustification               (juce::Justification::centredRight);
    bpmEditor.setFont                        (editorFont);

    bpmEditor.setColour(juce::TextEditor::backgroundColourId, res.theme.background);
    bpmEditor.setColour(juce::TextEditor::textColourId,       res.theme.textPrimary);
    bpmEditor.setColour(juce::TextEditor::highlightColourId,  res.theme.primaryAccent.withAlpha(0.4f));
    bpmEditor.setColour(juce::TextEditor::outlineColourId,    res.theme.primaryAccent);
    bpmEditor.setColour(juce::TextEditor::focusedOutlineColourId, res.theme.primaryAccent);

    bpmEditor.onReturnKey = [this]() { CommitBpmEdit(); };
    bpmEditor.onEscapeKey = [this]() { CancelBpmEdit(); };
    bpmEditor.onFocusLost = [this]() { CommitBpmEdit(); };

    addChildComponent(bpmEditor);
}

void TitleHeader::paint(juce::Graphics &g)
{
    const auto &theme = resources.theme;

    // === Left: plugin name + tagline ===
    g.setColour(theme.primaryAccent);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 13.0f, juce::Font::bold))
                 .withExtraKerningFactor(0.22f));
    g.drawText("DIRTY LITTLE BASS SYNTH", pluginNameRect, juce::Justification::centredLeft);

    g.setColour(theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                 .withExtraKerningFactor(0.12f));
    g.drawText(juce::String::fromUTF8("MONO \xc2\xb7 SINGLE VOICE"),
               taglineRect, juce::Justification::centredLeft);

    // === Centre: BPM display ===
    {
        const juce::String bpmText  = juce::String(currentTempo.bpm, 1) + " BPM";
        const juce::String sourceText = currentTempo.bpmFromHost ? "HOST" : "INT";
        const auto         srcColour  = currentTempo.bpmFromHost ? theme.secondaryAccent
                                                                 : theme.primaryAccent;

        // BPM number (prominent).
        g.setColour(theme.textPrimary);
        g.setFont(juce::Font(juce::FontOptions("Helvetica", 12.0f, juce::Font::bold))
                     .withExtraKerningFactor(0.10f));

        auto bpmTextRect = bpmRect.withTrimmedRight(48);     // leave room for the source badge
        g.drawText(bpmText, bpmTextRect, juce::Justification::centredRight);

        // Source badge.
        g.setColour(srcColour);
        g.setFont(juce::Font(juce::FontOptions("Helvetica", 8.0f, juce::Font::bold))
                     .withExtraKerningFactor(0.18f));

        auto badgeRect = bpmRect.withTrimmedLeft(bpmRect.getWidth() - 44);
        g.drawText(sourceText, badgeRect, juce::Justification::centredLeft);
    }

    // === Right: "Init Patch" label ===
    g.setColour(theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0))
                 .withExtraKerningFactor(0.10f));
    g.drawText("INIT PATCH", initPatchRect, juce::Justification::centredRight);

    // === Right: prev / next preset placeholder buttons ===
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
            arrow.addTriangle(cx + w * 0.5f, cy - h * 0.5f,
                              cx + w * 0.5f, cy + h * 0.5f,
                              cx - w * 0.5f, cy);
        else
            arrow.addTriangle(cx - w * 0.5f, cy - h * 0.5f,
                              cx - w * 0.5f, cy + h * 0.5f,
                              cx + w * 0.5f, cy);

        g.setColour(theme.textSecondary);
        g.fillPath(arrow);
    };

    drawArrowBtn(prevBtnRect, /*pointsLeft*/ true);
    drawArrowBtn(nextBtnRect, /*pointsLeft*/ false);

    // === Right: brand text ===
    g.setColour(theme.textSecondary);
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
    constexpr int bpmWidth  = 130;
    constexpr int bpmHeight = 18;

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

    // Centre: BPM display, vertically centred in the header.
    bpmRect = juce::Rectangle<int>((bounds.getWidth() - bpmWidth) / 2,
                                   bounds.getY() + (bounds.getHeight() - bpmHeight) / 2,
                                   bpmWidth, bpmHeight);

    // Left: plugin name + tagline. Right edge is the left edge of the BPM area.
    int leftX     = bounds.getX() + padding;
    int leftRight = bpmRect.getX() - 8;

    pluginNameRect = juce::Rectangle<int>(leftX, bounds.getY() + 12, leftRight - leftX, 18);
    taglineRect    = juce::Rectangle<int>(leftX, bounds.getY() + 32, leftRight - leftX, 14);
}

void TitleHeader::mouseDown(const juce::MouseEvent &e)
{
    // Only standalone mode allows BPM editing — and only when the cursor is
    // actually inside the BPM display area.
    if (! resources.isStandalone)
        return;

    if (! bpmRect.contains(e.getPosition()))
        return;

    if (auto *param = resources.apvts->getParameter("tempo_fallback_bpm"))
    {
        bpmDragging       = true;
        bpmDragStartY     = e.getPosition().y;
        bpmDragStartValue = currentTempo.bpm;
        param->beginChangeGesture();
    }
}

void TitleHeader::mouseDrag(const juce::MouseEvent &e)
{
    if (! bpmDragging)
        return;

    auto *param = resources.apvts->getParameter("tempo_fallback_bpm");
    if (param == nullptr)
        return;

    // Up = faster, down = slower. Shift = fine (×0.1).
    const float pixelsDelta = (float) (bpmDragStartY - e.getPosition().y);
    const float bpmPerPixel = e.mods.isShiftDown() ? 0.1f : 1.0f;

    const float newBpm = juce::jlimit(30.0f, 300.0f,
                                      bpmDragStartValue + pixelsDelta * bpmPerPixel);

    param->setValueNotifyingHost(param->convertTo0to1(newBpm));
}

void TitleHeader::mouseUp(const juce::MouseEvent &)
{
    if (! bpmDragging)
        return;

    if (auto *param = resources.apvts->getParameter("tempo_fallback_bpm"))
        param->endChangeGesture();

    bpmDragging = false;
}

void TitleHeader::mouseDoubleClick(const juce::MouseEvent &e)
{
    if (! resources.isStandalone)
        return;

    if (! bpmRect.contains(e.getPosition()))
        return;

    // Position the editor over the BPM number portion (leave the source badge visible).
    auto editArea = bpmRect.withTrimmedRight(48);

    bpmEditor.setBounds(editArea);
    bpmEditor.setText  (juce::String(currentTempo.bpm, 1), juce::dontSendNotification);
    bpmEditor.setVisible       (true);
    bpmEditor.grabKeyboardFocus();
}

void TitleHeader::CommitBpmEdit()
{
    if (! bpmEditor.isVisible())
        return;

    const float typed = bpmEditor.getText().getFloatValue();

    bpmEditor.setVisible(false);

    // Empty / unparseable input bails without writing.
    if (typed <= 0.0f)
        return;

    if (auto *param = resources.apvts->getParameter("tempo_fallback_bpm"))
    {
        const float clamped = juce::jlimit(30.0f, 300.0f, typed);

        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(clamped));
        param->endChangeGesture();
    }
}

void TitleHeader::CancelBpmEdit()
{
    bpmEditor.setVisible(false);
}

void TitleHeader::Update()
{
    if (resources.tempoSnapshot == nullptr)
        return;

    const auto latest = resources.tempoSnapshot->Read();

    // Only repaint when the displayed values actually change.
    const bool bpmChanged    = ! juce::approximatelyEqual(latest.bpm, currentTempo.bpm);
    const bool sourceChanged = latest.bpmFromHost != currentTempo.bpmFromHost;

    currentTempo = latest;

    if (bpmChanged || sourceChanged)
        repaint(bpmRect);
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
