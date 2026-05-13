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
    setOpaque(false);
    
    setInterceptsMouseClicks(true, false);
    setMouseCursor(juce::MouseCursor::CrosshairCursor);
    setWantsKeyboardFocus(false);
}

void MidiLearnOverlay::paint(juce::Graphics &g)
{
    if (! cacheBuilt)
        RebuildLearnableCache();

    // Subtle full-area tint to mark the mode visually.
    g.fillAll(theme.secondaryAccent.withAlpha(0.06f));

    PaintBadges(g);

    if (hoveredComp != nullptr)
        PaintHoverHalo(g, hoveredComp.getComponent());

    if (armedComp != nullptr && manager.GetState() == MidiLearnManager::State::Armed)
        PaintArmedPulse(g, armedComp.getComponent());

    if (flashComp != nullptr)
    {
        const auto elapsed = (juce::int64) juce::Time::getMillisecondCounter() - flashStartMs;
        if (elapsed < flashDurationMs)
            PaintFlash(g, flashComp.getComponent(), elapsed);
    }
}

void MidiLearnOverlay::mouseDown(const juce::MouseEvent &e)
{
    const auto editorPos = e.getPosition() + getPosition();

    auto *learnable = FindLearnableAt(editorPos);
    if (learnable == nullptr)
        return;

    const auto paramID = learnable->getProperties()["paramID"].toString();
    const int  idx     = manager.GetParamIndexById(paramID);
    if (idx < 0)
        return;

    // Right-click → context menu (unmap). Left-click → arm.
    if (e.mods.isPopupMenu())
    {
        ShowContextMenu(idx);
        return;
    }

    manager.ArmParam(idx);
    armedComp = learnable;
    repaint();
}

void MidiLearnOverlay::mouseMove(const juce::MouseEvent &e)
{
    const auto editorPos = e.getPosition() + getPosition();
    auto      *under     = FindLearnableAt(editorPos);

    if (hoveredComp.getComponent() != under)
    {
        hoveredComp = under;
        repaint();
    }
}

void MidiLearnOverlay::mouseExit(const juce::MouseEvent &)
{
    if (hoveredComp != nullptr)
    {
        hoveredComp = nullptr;
        repaint();
    }
}

void MidiLearnOverlay::Update()
{
    const auto state = manager.GetState();

    // Detect "binding just landed": Armed → Listening with a cached armed comp.
    if (lastState == MidiLearnManager::State::Armed
        && state    == MidiLearnManager::State::Listening
        && armedComp != nullptr)
    {
        flashComp    = armedComp;
        flashStartMs = (juce::int64) juce::Time::getMillisecondCounter();
    }

    // Drop the armed cache when state leaves Armed.
    if (state != MidiLearnManager::State::Armed)
        armedComp = nullptr;

    // Rebuild the badge cache after the state machine first becomes active —
    // ensures any param that gained a CC binding via persistence load shows up
    // immediately on first entering learn mode.
    if (lastState == MidiLearnManager::State::Idle && state != MidiLearnManager::State::Idle)
        cacheBuilt = false;

    lastState = state;

    // Animate while pulse or flash is active. Both demand per-frame repaints.
    const bool armedAnim = (state == MidiLearnManager::State::Armed && armedComp != nullptr);
    const bool flashing  = (flashComp != nullptr
                            && (juce::int64) juce::Time::getMillisecondCounter() - flashStartMs < flashDurationMs);

    if (armedAnim || flashing)
    {
        repaint();
    }
    else if (flashComp != nullptr)
    {
        // Flash duration just ended — clear the pointer and repaint once to
        // remove the residual visual.
        flashComp = nullptr;
        repaint();
    }
}

juce::Component *MidiLearnOverlay::FindLearnableAt(juce::Point<int> editorPos)
{
    auto *parent = getParentComponent();
    if (parent == nullptr)
        return nullptr;

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

void MidiLearnOverlay::ShowContextMenu(int paramIndex)
{
    juce::PopupMenu menu;

    const int cc = manager.GetFirstCcForParam(paramIndex);

    if (cc < 0)
    {
        // Disabled placeholder so the user gets visible feedback that there's
        // nothing to unmap on this param.
        menu.addItem("(not mapped)", /*isActive*/ false, /*isTicked*/ false, [](){});
    }
    else
    {
        const juce::String label = "Unmap CC" + juce::String(cc);

        juce::Component::SafePointer<MidiLearnOverlay> self(this);
        const int idxCopy = paramIndex;

        menu.addItem(label, [self, idxCopy]()
        {
            if (self != nullptr)
            {
                self->manager.UnmapParam(idxCopy);
                self->repaint();    // remove the CC# badge immediately
            }
        });
    }

    menu.showMenuAsync(juce::PopupMenu::Options{});
}

void MidiLearnOverlay::RebuildLearnableCache()
{
    learnableComps.clear();

    auto *editor = getParentComponent();
    if (editor == nullptr)
        return;

    std::function<void(juce::Component *)> walk = [&](juce::Component *c)
    {
        if (c == this || c == nullptr)
            return;

        if (c->getProperties().contains("paramID"))
            learnableComps.emplace_back(c);

        for (int i = 0; i < c->getNumChildComponents(); ++i)
            walk(c->getChildComponent(i));
    };

    walk(editor);
    cacheBuilt = true;
}

void MidiLearnOverlay::PaintBadges(juce::Graphics &g)
{
    const auto badgeFont = juce::Font(juce::FontOptions("Helvetica", 8.0f, juce::Font::bold))
                              .withExtraKerningFactor(0.10f);
    g.setFont(badgeFont);

    constexpr int badgeW = 30;
    constexpr int badgeH = 11;

    for (auto &sp : learnableComps)
    {
        auto *comp = sp.getComponent();
        if (comp == nullptr || ! comp->isVisible())
            continue;

        const auto paramID = comp->getProperties()["paramID"].toString();
        const int  paramIx = manager.GetParamIndexById(paramID);
        if (paramIx < 0)
            continue;

        const int cc = manager.GetFirstCcForParam(paramIx);
        if (cc < 0)
            continue;

        const auto compBounds = BoundsOf(comp);
        if (compBounds.isEmpty())
            continue;

        const auto badgeRect = juce::Rectangle<int>(compBounds.getRight() - badgeW - 2,
                                                    compBounds.getY()     + 2,
                                                    badgeW, badgeH);

        g.setColour(theme.background.withAlpha(0.85f));
        g.fillRoundedRectangle(badgeRect.toFloat(), 2.0f);

        g.setColour(theme.secondaryAccent);
        g.drawRoundedRectangle(badgeRect.toFloat().reduced(0.5f), 2.0f, 1.0f);

        g.setColour(theme.textPrimary);
        g.drawText("CC" + juce::String(cc), badgeRect, juce::Justification::centred);
    }
}

void MidiLearnOverlay::PaintHoverHalo(juce::Graphics &g, juce::Component *comp)
{
    if (comp == nullptr)
        return;

    const auto bounds = BoundsOf(comp).expanded(2);

    g.setColour(theme.secondaryAccent.withAlpha(hoverAlpha));
    g.drawRoundedRectangle(bounds.toFloat(), 4.0f, 1.2f);
}

void MidiLearnOverlay::PaintArmedPulse(juce::Graphics &g, juce::Component *comp)
{
    if (comp == nullptr)
        return;

    const auto bounds = BoundsOf(comp).expanded(3);

    // ~600ms cycle: alpha 0.6 ↔ 1.0
    const auto  now   = juce::Time::getMillisecondCounter();
    const float t     = (float) (now % 600) / 600.0f;
    const float alpha = 0.6f + 0.4f * (0.5f + 0.5f * std::sin(t * juce::MathConstants<float>::twoPi));

    g.setColour(theme.secondaryAccent.withAlpha(alpha));
    g.drawRoundedRectangle(bounds.toFloat(), 5.0f, 2.0f);
}

void MidiLearnOverlay::PaintFlash(juce::Graphics &g, juce::Component *comp, juce::int64 elapsedMs)
{
    if (comp == nullptr)
        return;

    const auto bounds = BoundsOf(comp).expanded(3);

    const float t     = juce::jlimit(0.0f, 1.0f, (float) elapsedMs / (float) flashDurationMs);
    const float alpha = (1.0f - t) * 0.55f;     // fades 0.55 → 0

    g.setColour(theme.secondaryAccent.withAlpha(alpha));
    g.fillRoundedRectangle(bounds.toFloat(), 5.0f);
}

juce::Rectangle<int> MidiLearnOverlay::BoundsOf(juce::Component *comp)
{
    if (comp == nullptr)
        return {};

    return getLocalArea(comp, comp->getLocalBounds());
}
