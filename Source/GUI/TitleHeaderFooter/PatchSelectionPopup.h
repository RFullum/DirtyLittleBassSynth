/*
  ==============================================================================

    PatchSelectionPopup.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"

//==============================================================================

/// Lists the available patches (factory first, then user) as a popup anchored
/// below the patch-name display in the title header. Backed by juce::PopupMenu
/// for now so we get auto-scrolling and screen-edge clamping for free; a
/// custom viewport replaces this later when richer per-item rendering is
/// wanted.
///
/// All operations are message-thread only. Construction is cheap (no state
/// beyond the GuiResources reference) — the menu is rebuilt on every Show().
class PatchSelectionPopup
{
public:
    explicit PatchSelectionPopup(GuiResources &resources);
    ~PatchSelectionPopup() = default;

    /// Opens the popup anchored below `targetComponent`'s screen area.
    /// Refreshes the patch list first so external changes (Finder drops,
    /// deletions) are always reflected without an explicit refresh button.
    /// Selection triggers PatchManager::LoadPatch on the chosen entry.
    void Show(juce::Component *targetComponent);

private:
    GuiResources &resources;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchSelectionPopup)
};
