/*
  ==============================================================================

    MasterChain.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

/// Post-voice master DSP block. Splits the stereo signal at a user-set crossover
/// frequency, forces the low band to mono (bass mono-izer), and applies a Haas
/// delay to one channel of the high band (stereo widener). The two bands sum back
/// to a flat response when slope-matched (Linkwitz-Riley LR4: two cascaded
/// 2nd-order Butterworth → 4th-order, 24 dB/oct).
///
/// `widthParam`: -1..+1, where 0 = mono (no Haas delay) and ±1 = max delay
/// (~25 ms). Sign selects which channel is delayed.
/// `monoCrossoverHzParam`: 20..300 Hz crossover frequency.
class MasterChain
{
public:
    MasterChain();

    /// Allocates the Haas delay buffer and prepares the LR filters. Must be
    /// called from prepareToPlay before any Process call.
    void Prepare(double sampleRate, int blockSize, int numChannels);

    /// Clears delay buffer + filter state. Call from prepareToPlay or on a
    /// transport reset.
    void Reset();

    /// Replaces the buffer in-place with the mono-izer + widener + limiter output.
    void Process(juce::AudioBuffer<float> &buffer);

    /// One-time setup: APVTS atomic pointers for widener + bass mono-izer.
    void SetParamPointers(std::atomic<float> *widthAmt, std::atomic<float> *monoCrossoverHz);

    /// One-time setup: APVTS atomic pointers for the limiter on/off + ceiling (dB).
    void SetLimiterParamPointers(std::atomic<float> *limiterOn, std::atomic<float> *ceilingDb);

    /// Latest gain-reduction value in dB (>= 0). Read by the UI thread to drive the
    /// GR meter; updated each block by the audio thread via a relaxed atomic store.
    float GetGainReductionDb() const noexcept { return currentGRDb.load(std::memory_order_relaxed); }

private:
    static constexpr float maxHaasDelaySec = 0.025f;

    std::atomic<float> *widthParam           = nullptr;
    std::atomic<float> *monoCrossoverHzParam = nullptr;
    std::atomic<float> *limiterOnParam       = nullptr;
    std::atomic<float> *limiterCeilingDbParam = nullptr;

    juce::dsp::LinkwitzRileyFilter<float> lowPass;
    juce::dsp::LinkwitzRileyFilter<float> highPass;

    juce::SmoothedValue<float> widthSmooth;
    juce::SmoothedValue<float> crossoverSmooth;

    // Power-of-two ring buffer for Haas delay (per channel).
    juce::AudioBuffer<float> haasBuffer;
    int writePos       = 0;
    int haasBufferSize = 0;

    // Limiter envelope-follower state. Coefficients computed in Prepare().
    float limiterEnvelope     = 0.0f;
    float limiterAttackCoeff  = 0.0f;
    float limiterReleaseCoeff = 0.0f;

    // Most recent gain reduction in dB, polled by the UI thread.
    std::atomic<float> currentGRDb { 0.0f };

    double sampleRate      = 44100.0;
    float  lastCrossoverHz = 120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterChain)
};
