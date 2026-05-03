/*
  ==============================================================================

    ScopeBuffer.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

/// Lock-free ring buffer that carries the master output's mono-summed samples
/// from the audio thread to the UI thread for the oscilloscope display. One
/// producer (audio) and one consumer (UI) — sufficient for a scope where slight
/// tearing between reads is visually acceptable.
class ScopeBuffer
{
public:
    static constexpr int bufferSize = 2048;   // power of two for cheap mask

    ScopeBuffer();

    /// Audio thread: pushes the per-sample mean of the input buffer's channels.
    void PushBlock(const juce::AudioBuffer<float> &source);

    /// UI thread: copies the latest `numSamples` samples in chronological order
    /// (oldest first) into `dest`. `numSamples` must be <= bufferSize.
    void CopyLatest(float *dest, int numSamples) const;

    void Clear();

private:
    float                samples[bufferSize] = {};
    std::atomic<int>     writeIndex          { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeBuffer)
};
