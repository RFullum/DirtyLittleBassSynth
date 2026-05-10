/*
  ==============================================================================

    PatchControls.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//==============================================================================

/// Title-header patch cluster: the "INIT PATCH" label + prev/next arrow
/// placeholders that will eventually drive patch save/recall. Currently
/// paint-only; behavior will be added when the patch system is implemented.
class PatchControls
    : public juce::Component
{
public:
    PatchControls(GuiResources &resources);
    ~PatchControls() override = default;

    void paint  (juce::Graphics &) override;
    void resized()                  override;

private:
    GuiResources &resources;

    juce::Rectangle<int> initPatchRect;
    juce::Rectangle<int> prevBtnRect;
    juce::Rectangle<int> nextBtnRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchControls)
};
