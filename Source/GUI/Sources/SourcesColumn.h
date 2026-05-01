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

//============================================================

/// Left column. Stacks the oscillator panel on top of the amp-ADSR panel and
/// owns the divider drawn between them. Spans the full body height between the
/// editor's header and footer.
class SourcesColumn
    : public juce::Component
{
public:
    SourcesColumn(GuiResources &res);

    void paint  (juce::Graphics &) override;
    void resized()                  override;

    /// Forwarded by the editor's timer.
    void Update();

private:
    GuiResources &resources;

    OscPanel     oscPanel;
    AmpAdsrPanel ampAdsrPanel;

    juce::Rectangle<int> rowDivider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourcesColumn)
};
