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

//============================================================

/// Filter column. Stacks the filter panel, the filter-ADSR panel, and the LFO
/// panel from top to bottom. Spans the full body height between header and footer.
class FilterColumn
    : public juce::Component
{
public:
    FilterColumn(GuiResources &res);

    void paint  (juce::Graphics &) override;
    void resized()                  override;

    /// Forwarded by the editor's timer.
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
