/*
  ==============================================================================

    MasterChain.h

  ==============================================================================
*/

#pragma once

#include "Filters/Crossover.h"
#include "Effects/HaasWidener.h"
#include "Effects/CeilingLimiter.h"
#include <JuceHeader.h>

//==============================================================================


class MasterChain
{
public:
    MasterChain();

    void Prepare(double sampleRate, int blockSize, int numChannels);
    void Reset();
    void Process(juce::AudioBuffer<float> &buffer);

    void SetParamPointers(std::atomic<float> *widthAmt, std::atomic<float> *monoCrossoverHz);
    void SetLimiterParamPointers(std::atomic<float> *limiterOn, std::atomic<float> *ceilingDb);
    
    float GetGainReductionDb() const noexcept { return limiter.GetGainReductionDb(); }

private:
    static constexpr float masterHpHz = 20.0f;
        juce::dsp::LinkwitzRileyFilter<float> masterHighPass;   // 20 Hz master HPF stays in DLBS

        FullumFilters::Crossover crossover;
        FullumFX::HaasWidener    haas;
        FullumFX::CeilingLimiter limiter;

        std::atomic<float> *widthParam            = nullptr;
        std::atomic<float> *monoCrossoverHzParam  = nullptr;
        std::atomic<float> *limiterOnParam        = nullptr;
        std::atomic<float> *limiterCeilingDbParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterChain)
};
