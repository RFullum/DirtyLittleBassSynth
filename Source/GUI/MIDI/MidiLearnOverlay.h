/*
  ==============================================================================

    MidiLearnOverlay.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "ColorPalette.h"
#include "MidiLearnManager.h"

//==============================================================================

// Glass-pane sized over the editor's body. In learn mode it intercepts clicks
// to learnable controls and paints the learn visuals (tint, hover halo, armed
// pulse, CC# badges, binding-lands flash).
class MidiLearnOverlay
    : public juce::Component
{
public:
    MidiLearnOverlay(MidiLearnManager &manager, const Palette::Theme &theme);

    void paint    (juce::Graphics &)         override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseMove(const juce::MouseEvent &) override;
    void mouseExit(const juce::MouseEvent &) override;

    // Polled from the editor's 60Hz timer while learn mode is active.
    void Update();

    void InvalidateCache() noexcept { cacheBuilt = false; }

private:
    juce::Component *FindLearnableAt(juce::Point<int> editorPos);
    static juce::Component *WalkUpForParamID(juce::Component *start);

    void ShowContextMenu(int paramIndex);

    void RebuildLearnableCache();

    void PaintBadges     (juce::Graphics &g);
    void PaintHoverHalo  (juce::Graphics &g, juce::Component *comp);
    void PaintArmedPulse (juce::Graphics &g, juce::Component *comp);
    void PaintFlash      (juce::Graphics &g, juce::Component *comp, juce::int64 elapsedMs);

    juce::Rectangle<int> BoundsOf(juce::Component *comp);

    MidiLearnManager     &manager;
    const Palette::Theme &theme;

    MidiLearnManager::State                          lastState     = MidiLearnManager::State::Idle;
    juce::Component::SafePointer<juce::Component>    hoveredComp;
    juce::Component::SafePointer<juce::Component>    armedComp;
    juce::Component::SafePointer<juce::Component>    flashComp;
    juce::int64                                      flashStartMs  = 0;

    std::vector<juce::Component::SafePointer<juce::Component>> learnableComps;
    bool                                              cacheBuilt   = false;

    static constexpr int    flashDurationMs = 280;
    static constexpr float  hoverAlpha      = 0.55f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiLearnOverlay)
};
