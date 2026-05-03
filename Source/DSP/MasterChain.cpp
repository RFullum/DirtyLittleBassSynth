/*
  ==============================================================================

    MasterChain.cpp

  ==============================================================================
*/

#include "MasterChain.h"

//==============================================================================

MasterChain::MasterChain()
{
    lowPass .setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    highPass.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

void MasterChain::Prepare(double sr, int blockSize, int numChannels)
{
    juce::ignoreUnused(blockSize);

    sampleRate = sr;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sr;
    spec.maximumBlockSize = (juce::uint32) blockSize;
    spec.numChannels      = (juce::uint32) juce::jmax(1, numChannels);

    lowPass .prepare(spec);
    highPass.prepare(spec);

    lowPass .setCutoffFrequency(lastCrossoverHz);
    highPass.setCutoffFrequency(lastCrossoverHz);

    crossoverSmooth.reset(sr, 0.05f);
    crossoverSmooth.setCurrentAndTargetValue(lastCrossoverHz);

    widthSmooth.reset(sr, 0.02f);
    widthSmooth.setCurrentAndTargetValue(0.0f);

    // Haas delay buffer: enough samples for maxHaasDelaySec at the current rate,
    // sized to the next power of two for cheap wraparound and a little headroom.
    const int maxDelaySamples = (int) std::ceil(maxHaasDelaySec * sr);
    haasBufferSize            = juce::nextPowerOfTwo(juce::jmax(64, maxDelaySamples + 16));

    haasBuffer.setSize(juce::jmax(1, numChannels), haasBufferSize, false, true, true);
    haasBuffer.clear();
    writePos = 0;

    // Limiter envelope coefficients. 2 ms attack snaps the envelope onto peaks
    // before they cross the ceiling; 80 ms release lets the gain return slowly so
    // limiting doesn't pump audibly.
    constexpr float limiterAttackSec  = 0.002f;
    constexpr float limiterReleaseSec = 0.080f;

    limiterAttackCoeff  = std::exp(-1.0f / (float) (sr * limiterAttackSec));
    limiterReleaseCoeff = std::exp(-1.0f / (float) (sr * limiterReleaseSec));
    limiterEnvelope     = 0.0f;
    currentGRDb.store(0.0f, std::memory_order_relaxed);
}

void MasterChain::Reset()
{
    lowPass .reset();
    highPass.reset();

    haasBuffer.clear();
    writePos = 0;

    widthSmooth    .setCurrentAndTargetValue(widthSmooth.getCurrentValue());
    crossoverSmooth.setCurrentAndTargetValue(crossoverSmooth.getCurrentValue());

    limiterEnvelope = 0.0f;
    currentGRDb.store(0.0f, std::memory_order_relaxed);
}

void MasterChain::Process(juce::AudioBuffer<float> &buffer)
{
    if (widthParam == nullptr || monoCrossoverHzParam == nullptr)
        return;

    const int numChannels = juce::jmin(2, buffer.getNumChannels());
    const int numSamples  = buffer.getNumSamples();

    if (numChannels < 1 || numSamples <= 0)
        return;

    // Update crossover frequency from the smoothed param. Block-rate is fine for
    // LR coefficient updates — the SmoothedValue glides between blocks so big
    // user movements still sound continuous.
    crossoverSmooth.setTargetValue(*monoCrossoverHzParam);

    const float blockCutoff = crossoverSmooth.skip(numSamples);
    if (std::abs(blockCutoff - lastCrossoverHz) > 0.5f)
    {
        lowPass .setCutoffFrequency(blockCutoff);
        highPass.setCutoffFrequency(blockCutoff);
        lastCrossoverHz = blockCutoff;
    }

    widthSmooth.setTargetValue(*widthParam);

    // Limiter setup (block-rate). When off, signal passes through unmodified and
    // the GR meter holds at 0 dB.
    const bool  limiterOn   = (limiterOnParam != nullptr) && (*limiterOnParam > 0.5f);
    const float ceilingDb   = (limiterCeilingDbParam != nullptr) ? limiterCeilingDbParam->load() : 0.0f;
    const float ceilingLin  = juce::Decibels::decibelsToGain(ceilingDb);

    float blockMaxGRDb = 0.0f;

    auto *bufL = buffer.getWritePointer(0);
    auto *bufR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    auto *delayL = haasBuffer.getWritePointer(0);
    auto *delayR = haasBuffer.getNumChannels() > 1 ? haasBuffer.getWritePointer(1) : delayL;

    const int  bufMask          = haasBufferSize - 1;
    const float maxDelaySamplesF = maxHaasDelaySec * (float) sampleRate;

    for (int i = 0; i < numSamples; ++i)
    {
        const float width = widthSmooth.getNextValue();

        const float inL = bufL[i];
        const float inR = (bufR != nullptr) ? bufR[i] : inL;

        // Band-split each channel.
        const float lowL  = lowPass .processSample(0, inL);
        const float highL = highPass.processSample(0, inL);
        const float lowR  = (bufR != nullptr) ? lowPass .processSample(1, inR) : lowL;
        const float highR = (bufR != nullptr) ? highPass.processSample(1, inR) : highL;

        // Force the low band to mono.
        const float lowMono = (lowL + lowR) * 0.5f;

        // Write the high band into the delay line, then read back with the
        // current Haas offset on whichever channel is being widened.
        delayL[writePos] = highL;
        delayR[writePos] = highR;

        const int   delaySamples = (int) (std::abs(width) * maxDelaySamplesF);
        const int   readPos      = (writePos - delaySamples) & bufMask;

        float outHighL = highL;
        float outHighR = highR;

        if (delaySamples > 0)
        {
            if (width > 0.0f)
                outHighL = delayL[readPos];   // slider right → delay left channel
            else
                outHighR = delayR[readPos];   // slider left  → delay right channel
        }

        writePos = (writePos + 1) & bufMask;

        float sampL = lowMono + outHighL;
        float sampR = lowMono + outHighR;

        // === Limiter (post mono+widen) ===
        if (limiterOn)
        {
            const float peak = juce::jmax(std::abs(sampL), std::abs(sampR));

            // Envelope follower: fast attack onto peaks, slow release.
            if (peak > limiterEnvelope)
                limiterEnvelope = limiterAttackCoeff  * (limiterEnvelope - peak) + peak;
            else
                limiterEnvelope = limiterReleaseCoeff * (limiterEnvelope - peak) + peak;

            const float gain = (limiterEnvelope > ceilingLin)
                                   ? (ceilingLin / juce::jmax(1.0e-9f, limiterEnvelope))
                                   : 1.0f;

            sampL *= gain;
            sampR *= gain;

            const float grDb = (gain < 1.0f)
                                   ? -juce::Decibels::gainToDecibels(juce::jmax(1.0e-6f, gain))
                                   : 0.0f;

            blockMaxGRDb = juce::jmax(blockMaxGRDb, grDb);
        }

        bufL[i] = sampL;
        if (bufR != nullptr)
            bufR[i] = sampR;
    }

    // Publish the block's peak gain reduction for the UI thread.
    currentGRDb.store(limiterOn
                        ? blockMaxGRDb
                        : 0.0f
                      , std::memory_order_relaxed);
}

void MasterChain::SetParamPointers(std::atomic<float> *widthAmt, std::atomic<float> *monoCrossover)
{
    widthParam           = widthAmt;
    monoCrossoverHzParam = monoCrossover;
}

void MasterChain::SetLimiterParamPointers(std::atomic<float> *limiterOn, std::atomic<float> *ceilingDb)
{
    limiterOnParam        = limiterOn;
    limiterCeilingDbParam = ceilingDb;
}
