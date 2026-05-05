/*
  ==============================================================================

    MidiLearnOverlay.cpp

  ==============================================================================
*/

#include "MidiLearnOverlay.h"

//============================================================

MidiLearnOverlay::MidiLearnOverlay(MidiLearnManager &m, const Palette::Theme &t)
: manager(m)
, theme  (t)
{
    setInterceptsMouseClicks(true, false);
}

void MidiLearnOverlay::paint(juce::Graphics &g)
{
    // Subtle full-area tint to indicate "you are in MIDI Learn mode". Faint
    // enough that the underlying UI stays fully readable.
    g.fillAll(theme.secondaryAccent.withAlpha(0.06f));
}

void MidiLearnOverlay::mouseDown(const juce::MouseEvent &e)
{
    // The event arrives in this overlay's coordinate space; translate back to
    // the parent (editor) space so we can ask "what would have been clicked
    // under here?" without our overlay in the way.
    const auto editorPos = e.getPosition() + getPosition();

    if (auto *learnable = FindLearnableAt(editorPos))
    {
        const auto paramID = learnable->getProperties()["paramID"].toString();
        const int  idx     = manager.GetParamIndexById(paramID);

        if (idx >= 0)
            manager.ArmParam(idx);
    }
}

//============================================================

juce::Component *MidiLearnOverlay::FindLearnableAt(juce::Point<int> editorPos)
{
    auto *parent = getParentComponent();
    if (parent == nullptr)
        return nullptr;

    // Make ourselves transparent to the hit-test for this one query, so the
    // editor's getComponentAt returns whatever sits below us at editorPos.
    setInterceptsMouseClicks(false, false);
    auto *under = parent->getComponentAt(editorPos);
    setInterceptsMouseClicks(true, false);

    return WalkUpForParamID(under);
}

juce::Component *MidiLearnOverlay::WalkUpForParamID(juce::Component *start)
{
    for (auto *c = start; c != nullptr; c = c->getParentComponent())
    {
        if (c->getProperties().contains("paramID"))
            return c;
    }

    return nullptr;
}
