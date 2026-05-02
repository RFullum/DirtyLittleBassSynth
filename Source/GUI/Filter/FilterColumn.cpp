/*
  ==============================================================================

    FilterColumn.cpp

  ==============================================================================
*/

#include "FilterColumn.h"

//============================================================

FilterColumn::FilterColumn(GuiResources &res)
: resources      (res)
, filterPanel    (res)
, filterAdsrPanel(res)
, lfoPanel       (res)
{
    addAndMakeVisible(filterPanel);
    addAndMakeVisible(filterAdsrPanel);
    addAndMakeVisible(lfoPanel);
}

void FilterColumn::paint(juce::Graphics &g)
{
    g.setColour(resources.theme.structure);
    g.fillRect(dividerA);
    g.fillRect(dividerB);
}

void FilterColumn::resized()
{
    constexpr int dividerThick = 1;
    constexpr int dividerInset = 8;

    auto bounds = getLocalBounds();

    // Equal-thirds split: filter / filter-ADSR / LFO.
    const int rowH    = bounds.getHeight() / 3;
    auto      filtArea = bounds.removeFromTop(rowH);
    auto      adsrArea = bounds.removeFromTop(rowH);

    filterPanel    .setBounds(filtArea);
    filterAdsrPanel.setBounds(adsrArea);
    lfoPanel       .setBounds(bounds);

    // Horizontal dividers between rows, inset from the column edges.
    dividerA = juce::Rectangle<int>(dividerInset,
                                    rowH - dividerThick / 2,
                                    getWidth() - dividerInset * 2,
                                    dividerThick);
    dividerB = juce::Rectangle<int>(dividerInset,
                                    rowH * 2 - dividerThick / 2,
                                    getWidth() - dividerInset * 2,
                                    dividerThick);
}

void FilterColumn::Update()
{
    filterPanel    .Update();
    filterAdsrPanel.Update();
    lfoPanel       .Update();
}
