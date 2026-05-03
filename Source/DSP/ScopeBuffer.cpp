/*
  ==============================================================================

    ScopeBuffer.cpp

  ==============================================================================
*/

#include "ScopeBuffer.h"

//==============================================================================

ScopeBuffer::ScopeBuffer()
{
    Clear();
}

void ScopeBuffer::PushBlock(const juce::AudioBuffer<float> &source)
{
    constexpr int mask = bufferSize - 1;

    const int numChannels = source.getNumChannels();
    const int numSamples  = source.getNumSamples();

    if (numChannels < 1 || numSamples <= 0)
        return;

    const auto *l = source.getReadPointer(0);
    const auto *r = numChannels > 1 ? source.getReadPointer(1) : nullptr;

    int wi = writeIndex.load(std::memory_order_relaxed);

    for (int i = 0; i < numSamples; ++i)
    {
        const float s = (r != nullptr) ? (l[i] + r[i]) * 0.5f : l[i];
        samples[wi] = s;
        wi = (wi + 1) & mask;
    }

    writeIndex.store(wi, std::memory_order_release);
}

void ScopeBuffer::CopyLatest(float *dest, int numSamples) const
{
    constexpr int mask = bufferSize - 1;

    if (numSamples <= 0 || numSamples > bufferSize)
        return;

    const int wi    = writeIndex.load(std::memory_order_acquire);
    const int start = (wi - numSamples) & mask;

    for (int i = 0; i < numSamples; ++i)
        dest[i] = samples[(start + i) & mask];
}

void ScopeBuffer::Clear()
{
    for (auto &s : samples)
        s = 0.0f;

    writeIndex.store(0, std::memory_order_release);
}
