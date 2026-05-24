/*
  ==============================================================================

    PatchSelectionPopup.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//==============================================================================

class PatchSelectionPopup
{
public:
    explicit PatchSelectionPopup(GuiResources &resources);
    ~PatchSelectionPopup() = default;

    void Show(juce::Component *targetComponent);

private:
    GuiResources &resources;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchSelectionPopup)
};
