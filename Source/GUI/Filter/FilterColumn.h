/*
  ==============================================================================

    FilterColumn.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "FilterPanel.h"
#include "FilterAdsrPanel.h"
#include "LfoPanel.h"

//==============================================================================

class FilterColumn
    : public juce::Component
{
public:
    FilterColumn(GuiResources &res);

    void paint  (juce::Graphics &) override;
    void resized()                  override;

    void Update();

private:
    GuiResources &resources;

    FilterPanel     filterPanel;
    FilterAdsrPanel filterAdsrPanel;
    LfoPanel        lfoPanel;

    juce::Rectangle<int> dividerA;
    juce::Rectangle<int> dividerB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterColumn)
};
