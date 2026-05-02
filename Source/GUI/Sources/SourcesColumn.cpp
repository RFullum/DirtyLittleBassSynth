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

    // Top: OscPanel. Bottom: AmpAdsrPanel. Equal split.
    const int topHeight = bounds.getHeight() / 2;
    auto      topArea   = bounds.removeFromTop(topHeight);

    oscPanel    .setBounds(topArea);
    ampAdsrPanel.setBounds(bounds);

    // Horizontal divider between the two panels, inset from the column edges.
    rowDivider = juce::Rectangle<int>(dividerInset,
                                      topHeight - dividerThick / 2,
                                      getWidth() - dividerInset * 2,
                                      dividerThick);
}

void SourcesColumn::Update()
{
    oscPanel    .Update();
    ampAdsrPanel.Update();
}
