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

void LfoPanel::Update()
{
    lfoVisual.Update();
}

void LfoPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int filtLabelHeight   = 30;

    auto area = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(area.removeFromTop(16).reduced(8, 0));

    auto reduced = area;

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

    lfoShapeLabel .setBounds(shapeLabelArea);
    lfoShapeSlider.setBounds(shapeSliderArea);
    lfoVisual     .setBounds(reduced);
}
