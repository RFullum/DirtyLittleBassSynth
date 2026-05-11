/*
  ==============================================================================

    PatchSelectionPopup.cpp

  ==============================================================================
*/

#include "PatchSelectionPopup.h"

//==============================================================================

PatchSelectionPopup::PatchSelectionPopup(GuiResources &res)
: resources(res)
{}

void PatchSelectionPopup::Show(juce::Component *targetComponent)
{
    if (resources.patchManager == nullptr || targetComponent == nullptr)
        return;

    // Refresh on every open so external Finder changes (drops, deletions)
    // are reflected without needing a manual refresh action.
    auto *pm = resources.patchManager;
    pm->RefreshPatchList();

    const auto &theme       = resources.theme;
    const auto &patches     = pm->GetPatchList();
    const auto  currentFile = pm->GetCurrentPatchFile();

    juce::PopupMenu menu;

    // Empty list: a single disabled placeholder so the popup doesn't appear
    // broken. juce::PopupMenu can do strange things on some platforms when
    // shown completely empty.
    if (patches.empty())
    {
        menu.addItem(-1, "(no patches yet)", /*isEnabled*/ false);
    }
    else
    {
        // Walk the list once, splitting at the factory/user boundary so we
        // can drop a separator between the two sections. Item IDs are 1-based
        // because PopupMenu returns 0 for "dismissed" — we need to distinguish
        // "user picked the first patch" from "user pressed Esc".
        bool factorySeen   = false;
        bool separatorAdded = false;

        for (size_t i = 0; i < patches.size(); ++i)
        {
            const auto &info      = patches[i];
            const bool  isFactory = info.source == PatchManager::Source::Factory;

            // Drop the separator the moment we transition factory -> user.
            if (! isFactory && factorySeen && ! separatorAdded)
            {
                menu.addSeparator();
                separatorAdded = true;
            }

            juce::PopupMenu::Item item;
            item.itemID   = (int) (i + 1);
            item.text     = info.name;
            item.colour   = isFactory ? theme.primaryAccent : theme.secondaryAccent;
            item.isTicked = (info.file == currentFile);
            menu.addItem(item);

            if (isFactory)
                factorySeen = true;
        }
    }

    // Snapshot the file list so the callback isn't sensitive to later
    // RefreshPatchList calls (e.g. from drag-drop import landing between the
    // popup opening and the user selecting an entry).
    std::vector<juce::File> fileSnapshot;
    fileSnapshot.reserve(patches.size());
    for (const auto &p : patches)
        fileSnapshot.push_back(p.file);

    const auto options = juce::PopupMenu::Options()
                             .withTargetComponent(targetComponent)
                             .withTargetScreenArea(targetComponent->getScreenBounds());

    menu.showMenuAsync(options, [pm, fileSnapshot = std::move(fileSnapshot)] (int chosen)
    {
        if (chosen <= 0)
            return;   // 0 = dismissed, -1 = the empty-list placeholder

        const auto index = (size_t) (chosen - 1);
        if (index < fileSnapshot.size())
            pm->LoadPatch(fileSnapshot[index]);
    });
}
