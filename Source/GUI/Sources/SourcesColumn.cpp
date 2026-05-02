/*
  ==============================================================================

    SourcesColumn.cpp

  ==============================================================================
*/

#include "SourcesColumn.h"

//============================================================

SourcesColumn::SourcesColumn(GuiResources &res)
: resources   (res)
, oscPanel    (res)
, ampAdsrPanel(res)
{
    addAndMakeVisible(oscPanel);
    addAndMakeVisible(ampAdsrPanel);
}

void SourcesColumn::paint(juce::Graphics &g)
{
    g.setColour(resources.theme.structure);
    g.fillRect(rowDivider);
}

void SourcesColumn::resized()
{
    constexpr int dividerThick = 1;
    constexpr int dividerInset = 8;

    auto bounds = getLocalBounds();
    oscPanel.setBounds(bounds.removeFromTop(bounds.proportionOfHeight(0.666f)));
    rowDivider = bounds.removeFromTop(dividerThick).reduced(dividerInset, 0);
    ampAdsrPanel.setBounds(bounds);
}

void SourcesColumn::Update()
{
    oscPanel    .Update();
    ampAdsrPanel.Update();
}
