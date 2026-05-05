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

//============================================================

/// Glass-pane overlay sized over the editor's body area. Hidden in idle mode;
/// when MIDI Learn is active it becomes visible and intercepts every mouse
/// click that would otherwise reach a learnable control. Also paints all the
/// learn-mode visuals: subtle global tint, hover halo, armed-param pulse,
/// CC# badges on mapped params, and a transient flash when a binding lands.
class MidiLearnOverlay
    : public juce::Component
{
public:
    MidiLearnOverlay(MidiLearnManager &manager, const Palette::Theme &theme);

    void paint    (juce::Graphics &)         override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseMove(const juce::MouseEvent &) override;
    void mouseExit(const juce::MouseEvent &) override;

    /// Polled from the editor's 60Hz timer when learn mode is active.
    /// Detects state transitions (Armed → Listening = binding landed, kicks
    /// off a flash) and triggers repaints while animations are in progress.
    void Update();

    /// Forces the cache of learnable components to be rebuilt on the next
    /// paint. Currently unused — components are static after editor construction
    /// — but available for future dynamic-UI support.
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

    // Animation / transition state.
    MidiLearnManager::State                          lastState     = MidiLearnManager::State::Idle;
    juce::Component::SafePointer<juce::Component>    hoveredComp;
    juce::Component::SafePointer<juce::Component>    armedComp;
    juce::Component::SafePointer<juce::Component>    flashComp;
    juce::int64                                      flashStartMs  = 0;

    // Cache of all learnable child components, populated lazily on first paint.
    std::vector<juce::Component::SafePointer<juce::Component>> learnableComps;
    bool                                              cacheBuilt   = false;

    static constexpr int    flashDurationMs = 280;
    static constexpr float  hoverAlpha      = 0.55f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiLearnOverlay)
};
