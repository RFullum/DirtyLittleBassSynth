/*
  ==============================================================================

    MidiLearnManager.cpp

  ==============================================================================
*/

#include "MidiLearnManager.h"

//==============================================================================

MidiLearnManager::MidiLearnManager()
{
    for (auto &slot : ccToParamIndex)
        slot.store(-1, std::memory_order_relaxed);
}

//==============================================================================

int MidiLearnManager::RegisterParam(juce::AudioProcessorValueTreeState &apvts, const juce::String &paramID)
{
    auto *param = apvts.getParameter(paramID);
    if (param == nullptr)
    {
        jassertfalse;       // paramID typo or APVTS not yet built
        return -1;
    }

    Entry entry;
    entry.param   = param;
    entry.paramID = paramID;

    params.push_back(std::move(entry));
    return (int) params.size() - 1;
}

//==============================================================================

void MidiLearnManager::HandleControllerMessage(int ccNumber, int ccValue)
{
    if (ccNumber < 0 || ccNumber >= numCcSlots)
        return;

    // === Learn mode: bind the incoming CC to the armed param. ===
    if (state.load(std::memory_order_acquire) == State::Armed)
    {
        const int armed = armedParamIndex.load(std::memory_order_acquire);

        if (armed >= 0 && armed < (int) params.size())
        {
            ccToParamIndex[(size_t) ccNumber].store(armed, std::memory_order_release);
            dirty          .store(true,              std::memory_order_release);

            // Drop the armed flag and return to Listening so the user can
            // immediately click another param without re-toggling learn mode.
            armedParamIndex.store(-1,                std::memory_order_release);
            state          .store(State::Listening,  std::memory_order_release);
            return;
        }
    }

    // === Normal operation: route the CC to whatever param it's mapped to. ===
    const int paramIdx = ccToParamIndex[(size_t) ccNumber].load(std::memory_order_acquire);

    if (paramIdx >= 0 && paramIdx < (int) params.size())
    {
        if (auto *param = params[(size_t) paramIdx].param)
        {
            const float normalised = (float) ccValue / 127.0f;
            param->setValueNotifyingHost(normalised);
        }
    }
}

//==============================================================================

void MidiLearnManager::EnterListening() noexcept
{
    armedParamIndex.store(-1,               std::memory_order_release);
    state          .store(State::Listening, std::memory_order_release);
}

void MidiLearnManager::ExitLearn() noexcept
{
    armedParamIndex.store(-1,          std::memory_order_release);
    state          .store(State::Idle, std::memory_order_release);
}

void MidiLearnManager::ArmParam(int paramIndex) noexcept
{
    if (paramIndex < 0 || paramIndex >= (int) params.size())
        return;

    if (state.load(std::memory_order_acquire) == State::Idle)
        return;     // ignore arming when not in learn mode

    armedParamIndex.store(paramIndex,    std::memory_order_release);
    state          .store(State::Armed,  std::memory_order_release);
}

//==============================================================================

void MidiLearnManager::SetMapping(int ccNumber, int paramIndex) noexcept
{
    if (ccNumber < 0 || ccNumber >= numCcSlots)
        return;

    // paramIndex out of range collapses to "unmapped" (-1).
    const int idx = (paramIndex >= 0 && paramIndex < (int) params.size()) ? paramIndex : -1;
    ccToParamIndex[(size_t) ccNumber].store(idx, std::memory_order_release);
    dirty                              .store(true, std::memory_order_release);
}

void MidiLearnManager::UnmapParam(int paramIndex) noexcept
{
    if (paramIndex < 0 || paramIndex >= (int) params.size())
        return;

    bool changed = false;

    for (auto &slot : ccToParamIndex)
    {
        if (slot.load(std::memory_order_acquire) == paramIndex)
        {
            slot.store(-1, std::memory_order_release);
            changed = true;
        }
    }

    if (changed)
        dirty.store(true, std::memory_order_release);
}

void MidiLearnManager::ClearAllMappings() noexcept
{
    for (auto &slot : ccToParamIndex)
        slot.store(-1, std::memory_order_release);

    dirty.store(true, std::memory_order_release);
}

//==============================================================================

int MidiLearnManager::GetParamForCc(int ccNumber) const noexcept
{
    if (ccNumber < 0 || ccNumber >= numCcSlots)
        return -1;

    return ccToParamIndex[(size_t) ccNumber].load(std::memory_order_acquire);
}

int MidiLearnManager::GetFirstCcForParam(int paramIndex) const noexcept
{
    if (paramIndex < 0 || paramIndex >= (int) params.size())
        return -1;

    for (int cc = 0; cc < numCcSlots; ++cc)
    {
        if (ccToParamIndex[(size_t) cc].load(std::memory_order_acquire) == paramIndex)
            return cc;
    }

    return -1;
}

//==============================================================================

juce::String MidiLearnManager::GetParamID(int paramIndex) const
{
    if (paramIndex < 0 || paramIndex >= (int) params.size())
        return {};

    return params[(size_t) paramIndex].paramID;
}

juce::RangedAudioParameter* MidiLearnManager::GetParam(int paramIndex) const noexcept
{
    if (paramIndex < 0 || paramIndex >= (int) params.size())
        return nullptr;

    return params[(size_t) paramIndex].param;
}

int MidiLearnManager::GetParamIndexById(const juce::String &paramID) const
{
    for (size_t i = 0; i < params.size(); ++i)
    {
        if (params[i].paramID == paramID)
            return (int) i;
    }

    return -1;
}

//==============================================================================

juce::String MidiLearnManager::SerialiseMappings() const
{
    juce::StringArray entries;

    for (int cc = 0; cc < numCcSlots; ++cc)
    {
        const int idx = ccToParamIndex[(size_t) cc].load(std::memory_order_acquire);

        if (idx >= 0 && idx < (int) params.size())
            entries.add(juce::String(cc) + ":" + params[(size_t) idx].paramID);
    }

    return entries.joinIntoString("|");
}

void MidiLearnManager::RestoreMappings(const juce::String &serialised)
{
    ClearAllMappings();

    auto entries = juce::StringArray::fromTokens(serialised, "|", "");

    for (const auto &entry : entries)
    {
        const int colon = entry.indexOfChar(':');
        if (colon <= 0)
            continue;

        const int          cc      = entry.substring(0, colon).getIntValue();
        const juce::String paramID = entry.substring(colon + 1);
        const int          idx     = GetParamIndexById(paramID);

        if (cc >= 0 && cc < numCcSlots && idx >= 0)
            ccToParamIndex[(size_t) cc].store(idx, std::memory_order_release);
    }

    // Loading from disk shouldn't immediately schedule a save — clear the
    // dirty flag that ClearAllMappings set above.
    ClearDirtyFlag();
}
