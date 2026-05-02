/*
  ==============================================================================

    LfoPanel.cpp

  ==============================================================================
*/

#include "LfoPanel.h"
#include "GuiHelpers.h"

//============================================================

LfoPanel::LfoPanel(GuiResources &res)
: resources(res)
{
    using namespace dlbs;

    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    SetupSlider(this, lfoShapeSlider,  juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    SetupSlider(this, lfoFreqSlider,   juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    SetupSlider(this, lfoAmountSlider, juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    // LFO shape slider sits directly under the LFO visual; suppress its value textbox
    // so the slider track spans the full width of the visual above it.
    lfoShapeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    lfoShapeSlider .setLookAndFeel(res.dialLookAndFeel);
    lfoFreqSlider  .setLookAndFeel(res.dialLookAndFeel);
    lfoAmountSlider.setLookAndFeel(res.dialLookAndFeel);

    SetupSectionLabel(this, sectionLabel, "Filter LFO", res.theme.textSecondary);

    SetupLabel(this, lfoShapeLabel,  "LFO Shape", txt, 17.0f);
    SetupLabel(this, lfoFreqLabel,   "Freq",      txt, 16.0f);
    SetupLabel(this, lfoAmountLabel, "Amount",    txt, 16.0f);

    shapeAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtLFO_shape", lfoShapeSlider);
    freqAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtLFO_freq",  lfoFreqSlider);
    amountAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtLFO_amt",   lfoAmountSlider);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    lfoVisual.SetColors(accent, bg, bgFade);
    lfoVisual.Init(res.apvts->getRawParameterValue("filtLFO_shape"), /*useSquare*/ true);

    addAndMakeVisible(lfoVisual);
}

void LfoPanel::paint(juce::Graphics &g)
{
    // Decorative ms / sync pill toggle and time display. Non-interactive placeholder
    // for the future tempo-sync feature.
    constexpr float corner = 2.0f;

    // "MS" segment (active / on-state)
    g.setColour(resources.theme.secondaryAccent.withAlpha(0.15f));
    g.fillRoundedRectangle(syncMsRect.toFloat(), corner);
    g.setColour(resources.theme.secondaryAccent);
    g.drawRoundedRectangle(syncMsRect.toFloat().reduced(0.5f), corner, 1.0f);

    g.setFont(juce::Font(juce::FontOptions("Helvetica", 8.0f, juce::Font::bold))
                 .withExtraKerningFactor(0.10f));
    g.drawText("MS", syncMsRect, juce::Justification::centred);

    // "SYNC" segment (inactive / dim)
    g.setColour(resources.theme.structure);
    g.drawRoundedRectangle(syncSyncRect.toFloat().reduced(0.5f), corner, 1.0f);
    g.setColour(resources.theme.textSecondary);
    g.drawText("SYNC", syncSyncRect, juce::Justification::centred);

    // Time display placeholder
    g.setColour(resources.theme.textSecondary.withAlpha(0.5f));
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 9.0f, 0)));
    g.drawText(juce::String::fromUTF8("\xe2\x80\x94 ms"), syncDisplayRect, juce::Justification::centredRight);
}

void LfoPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int filtLabelHeight   = 30;
    constexpr int syncRowHeight     = 24;
    constexpr int pillWidth         = 80;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(bounds.removeFromTop(16).reduced(8, 0));

    auto reduced = bounds;

    auto vertSliderArea  = reduced.removeFromRight(reduced.getWidth() / 3);
    auto vertLabelFooter = vertSliderArea.removeFromTop(filtLabelHeight);
    auto freqLabelArea   = vertLabelFooter.removeFromLeft(vertLabelFooter.getWidth() / 2);

    lfoFreqLabel  .setBounds(freqLabelArea);
    lfoAmountLabel.setBounds(vertLabelFooter);

    auto freqSliderArea = vertSliderArea.removeFromLeft(vertSliderArea.getWidth() / 2);
    lfoFreqSlider  .setBounds(freqSliderArea);
    lfoAmountSlider.setBounds(vertSliderArea);

    auto shapeLabelArea  = reduced.removeFromBottom(filtLabelHeight);
    auto shapeSliderArea = reduced.removeFromBottom(filtLabelHeight);
    auto syncRow         = reduced.removeFromBottom(syncRowHeight).reduced(8, 4);

    lfoShapeLabel .setBounds(shapeLabelArea);
    lfoShapeSlider.setBounds(shapeSliderArea);

    // Sync pill (left) + time display (right) within syncRow.
    auto pillArea     = syncRow.removeFromLeft(pillWidth);
    syncMsRect        = pillArea.removeFromLeft(pillArea.getWidth() / 2);
    syncSyncRect      = pillArea;
    syncDisplayRect   = syncRow;

    lfoVisual     .setBounds(reduced);
}

void LfoPanel::Update()
{
    lfoVisual.Update();
}
