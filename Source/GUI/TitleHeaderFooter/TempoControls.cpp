/*
  ==============================================================================

    TempoControls.cpp

  ==============================================================================
*/

#include "TempoControls.h"

//==============================================================================

TempoControls::TempoControls(GuiResources &res)
: resources(res)
{
    // Inline BPM text editor: hidden by default; appears on double-click in
    // standalone mode for direct typing of the tempo value.
    const auto editorFont = juce::Font(juce::FontOptions("Helvetica"
                                                         , 12.0f
                                                         , juce::Font::bold))
                                        .withExtraKerningFactor(0.10f);

    bpmEditor.setMultiLine                   (false);
    bpmEditor.setReturnKeyStartsNewLine      (false);
    bpmEditor.setEscapeAndReturnKeysConsumed (true);
    bpmEditor.setSelectAllWhenFocused        (true);
    bpmEditor.setInputRestrictions           (8, "0123456789.");
    bpmEditor.setBorder                      (juce::BorderSize<int>(0));
    bpmEditor.setJustification               (juce::Justification::centredRight);
    bpmEditor.setFont                        (editorFont);

    bpmEditor.setColour(juce::TextEditor::backgroundColourId,     res.theme.background);
    bpmEditor.setColour(juce::TextEditor::textColourId,           res.theme.textPrimary);
    bpmEditor.setColour(juce::TextEditor::highlightColourId,      res.theme.primaryAccent.withAlpha(0.4f));
    bpmEditor.setColour(juce::TextEditor::outlineColourId,        res.theme.primaryAccent);
    bpmEditor.setColour(juce::TextEditor::focusedOutlineColourId, res.theme.primaryAccent);

    bpmEditor.onReturnKey = [this]() { CommitBpmEdit(); };
    bpmEditor.onEscapeKey = [this]() { CancelBpmEdit(); };
    bpmEditor.onFocusLost = [this]() { CommitBpmEdit(); };

    addChildComponent(bpmEditor);
}

void TempoControls::paint(juce::Graphics &g)
{
    const auto &theme  = resources.theme;
    auto        bounds = getLocalBounds();

    const juce::String bpmText    = juce::String(currentTempo.bpm, 1) + " BPM";
    const juce::String sourceText = currentTempo.bpmFromHost
                                    ? "HOST"
                                    : "INT";
    const auto         srcColour  = currentTempo.bpmFromHost
                                    ? theme.secondaryAccent
                                    : theme.primaryAccent;

    // BPM number (prominent).
    g.setColour(theme.textPrimary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 12.0f, juce::Font::bold)).withExtraKerningFactor(0.10f));

    auto bpmTextRect = bounds.withTrimmedRight(48);     // leave room for the source badge
    g.drawText(bpmText, bpmTextRect, juce::Justification::centredRight);

    // Source badge.
    g.setColour(srcColour);
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 8.0f, juce::Font::bold)).withExtraKerningFactor(0.18f));

    auto badgeRect = bounds.withTrimmedLeft(bounds.getWidth() - 44);
    g.drawText(sourceText, badgeRect, juce::Justification::centredLeft);
}

void TempoControls::resized()
{
    // Editor occupies the BPM-number area (leaving the source badge visible)
    // when made visible by mouseDoubleClick.
    bpmEditor.setBounds(getLocalBounds().withTrimmedRight(48));
}

void TempoControls::mouseDown(const juce::MouseEvent &e)
{
    // Only standalone mode allows BPM editing.
    if (! resources.isStandalone)
        return;

    if (auto *param = resources.apvts->getParameter("tempo_fallback_bpm"))
    {
        bpmDragging       = true;
        bpmDragStartY     = e.getPosition().y;
        bpmDragStartValue = currentTempo.bpm;
        param->beginChangeGesture();
    }
}

void TempoControls::mouseDrag(const juce::MouseEvent &e)
{
    if (! bpmDragging)
        return;

    auto *param = resources.apvts->getParameter("tempo_fallback_bpm");
    if (param == nullptr)
        return;

    // Up = faster, down = slower. Shift = fine (×0.1).
    const float pixelsDelta = (float) (bpmDragStartY - e.getPosition().y);
    const float bpmPerPixel = e.mods.isShiftDown() ? 0.1f : 1.0f;

    const float newBpm = juce::jlimit(30.0f, 300.0f, bpmDragStartValue + pixelsDelta * bpmPerPixel);

    param->setValueNotifyingHost(param->convertTo0to1(newBpm));
}

void TempoControls::mouseUp(const juce::MouseEvent &)
{
    if (! bpmDragging)
        return;

    if (auto *param = resources.apvts->getParameter("tempo_fallback_bpm"))
        param->endChangeGesture();

    bpmDragging = false;
}

void TempoControls::mouseDoubleClick(const juce::MouseEvent &)
{
    if (! resources.isStandalone)
        return;

    bpmEditor.setText(juce::String(currentTempo.bpm, 1), juce::dontSendNotification);
    bpmEditor.setVisible(true);
    bpmEditor.grabKeyboardFocus();
}

void TempoControls::CommitBpmEdit()
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

void TempoControls::CancelBpmEdit()
{
    bpmEditor.setVisible(false);
}

void TempoControls::Update()
{
    if (resources.tempoSnapshot == nullptr)
        return;

    const auto latest = resources.tempoSnapshot->Read();

    // Only repaint when the displayed values actually change.
    const bool bpmChanged    = ! juce::approximatelyEqual(latest.bpm, currentTempo.bpm);
    const bool sourceChanged = latest.bpmFromHost != currentTempo.bpmFromHost;

    currentTempo = latest;

    if (bpmChanged || sourceChanged)
        repaint();
}
