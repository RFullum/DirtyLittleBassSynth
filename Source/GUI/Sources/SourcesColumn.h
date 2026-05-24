/*
  ==============================================================================

    SourcesColumn.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OscPanel.h"
#include "AmpAdsrPanel.h"

//==============================================================================

class SourcesColumn
    : public juce::Component
{
public:
    SourcesColumn(GuiResources &res);

    void paint  (juce::Graphics &) override;
    void resized()                  override;

    void Update();

private:
    GuiResources &resources;

    OscPanel     oscPanel;
    AmpAdsrPanel ampAdsrPanel;

    juce::Rectangle<int> rowDivider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourcesColumn)
};
