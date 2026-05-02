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
    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    // TODO: Remove Freq & Amount slider labels. Create a way to display frequency or sync'd subdivision
    DLBS::SetupSlider(this, lfoShapeSlider,  juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    DLBS::SetupSlider(this, lfoFreqSlider,   juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    DLBS::SetupSlider(this, lfoAmountSlider, juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    // LFO shape slider sits directly under the LFO visual; suppress its value textbox
    // so the slider track spans the full width of the visual above it.
    lfoShapeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    lfoShapeSlider .setLookAndFeel(res.dialLookAndFeel);
    lfoFreqSlider  .setLookAndFeel(res.dialLookAndFeel);
    lfoAmountSlider.setLookAndFeel(res.dialLookAndFeel);

    DLBS::SetupSectionLabel(this, sectionLabel, "Filter LFO", res.theme.textSecondary);

    DLBS::SetupLabel(this, lfoFreqLabel,   "Freq",      txt, 14.0f);
    DLBS::SetupLabel(this, lfoAmountLabel, "Amount",    txt, 14.0f);

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
    g.fillRoundedRectangle(syncFrqRect.toFloat(), corner);
    g.setColour(resources.theme.secondaryAccent);
    g.drawRoundedRectangle(syncFrqRect.toFloat().reduced(0.5f), corner, 1.0f);

    g.setFont(juce::Font(juce::FontOptions("Helvetica", 10.0f, juce::Font::bold))
                 .withExtraKerningFactor(0.10f));
    g.drawText("FRQ", syncFrqRect, juce::Justification::centred);

    // "SYNC" segment (inactive / dim)
    g.setColour(resources.theme.structure);
    g.drawRoundedRectangle(syncSyncRect.toFloat().reduced(0.5f), corner, 1.0f);
    g.setColour(resources.theme.textSecondary);
    g.drawText("SYNC", syncSyncRect, juce::Justification::centred);

    // Time display placeholder
    g.setColour(resources.theme.textSecondary.withAlpha(0.5f));
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 10.0f, 0)));
    g.drawText(juce::String::fromUTF8("\xe2\x80\x94 Hz"), syncDisplayRect, juce::Justification::centredRight);
}

void LfoPanel::resized()
{
    static constexpr int sectionSpacerSize = 2;
    static constexpr int sliderLabelH      = 13;
    
    auto bounds = getLocalBounds().reduced(sectionSpacerSize);
    sectionLabel.setBounds(bounds.removeFromTop(16));
    bounds.removeFromBottom(sectionSpacerSize);
    
    auto slidersArea = bounds.removeFromRight(100);
    auto amtArea = slidersArea.removeFromRight(slidersArea.proportionOfWidth(0.5f));
    lfoAmountLabel .setBounds(amtArea.removeFromTop(sliderLabelH));
    lfoAmountSlider.setBounds(amtArea);
    lfoFreqLabel   .setBounds(slidersArea.removeFromTop(sliderLabelH));
    lfoFreqSlider  .setBounds(slidersArea);
    
    auto      syncRow      = bounds.removeFromBottom(25).reduced(sectionSpacerSize * 2, sectionSpacerSize);
    const int syncDivision = (syncRow.getWidth() - (sectionSpacerSize * 2)) / 3;
    syncFrqRect     = syncRow.removeFromLeft(syncDivision);
    syncRow         .removeFromLeft(sectionSpacerSize);
    syncSyncRect    = syncRow.removeFromLeft(syncDivision);
    syncRow         .removeFromLeft(sectionSpacerSize);
    syncDisplayRect = syncRow;
    
    lfoShapeSlider.setBounds(bounds.removeFromBottom(30));
    lfoVisual     .setBounds(bounds.reduced(sectionSpacerSize));
}

void LfoPanel::Update()
{
    lfoVisual.Update();
}
