/*
  ==============================================================================

    ModifiersColumn.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "ModifierPanel.h"

//============================================================

/// Modifiers column. Currently houses just the ModifierPanel; wraps it as a
/// column so the editor lays out four equal-width column components.
class ModifiersColumn
    : public juce::Component
{
public:
    ModifiersColumn(GuiResources &res);

    void resized() override;

private:
    GuiResources &resources;

    ModifierPanel modifierPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModifiersColumn)
};
