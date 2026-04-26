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

    SetupSlider(this, lfoShapeSlider,  juce::Slider::SliderStyle::LinearHorizontal, res.magicMint,  res.magicMint,  res.textColor, false);
    SetupSlider(this, lfoFreqSlider,   juce::Slider::SliderStyle::LinearVertical,   res.fieryRose,  res.fieryRose,  res.textColor, false);
    SetupSlider(this, lfoAmountSlider, juce::Slider::SliderStyle::LinearVertical,   res.orangePeel, res.orangePeel, res.textColor, false);

    lfoShapeSlider .setLookAndFeel(res.dialLookAndFeel);
    lfoFreqSlider  .setLookAndFeel(res.dialLookAndFeel);
    lfoAmountSlider.setLookAndFeel(res.dialLookAndFeel);

    SetupLabel(this, lfoShapeLabel,  "LFO Shape", res.textColor, 17.0f);
    SetupLabel(this, lfoFreqLabel,   "Freq",      res.textColor, 16.0f);
    SetupLabel(this, lfoAmountLabel, "Amount",    res.textColor, 16.0f);

    shapeAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtLFO_shape", lfoShapeSlider);
    freqAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtLFO_freq",  lfoFreqSlider);
    amountAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "filtLFO_amt",   lfoAmountSlider);

    juce::Colour onyxDark = res.onyx.darker().darker();
    lfoVisual.SetColors(res.magicMint, res.onyx, onyxDark);
    lfoVisual.Init(res.apvts->getRawParameterValue("filtLFO_shape"), /*useSquare*/ true);

    addAndMakeVisible(lfoVisual);
}

void LfoPanel::paint(juce::Graphics &g)
{
    constexpr float cornerRound = 2.0f;

    g.setColour(resources.magicMint);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRound);

    g.setColour(resources.onyx);
    g.fillRoundedRectangle(innerBg, cornerRound);
}

void LfoPanel::Update()
{
    lfoVisual.Update();
}

void LfoPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int filtLabelHeight   = 30;

    auto area    = getLocalBounds().reduced(sectionSpacerSize);
    auto reduced = area;
    innerBg = reduced.toFloat();

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
