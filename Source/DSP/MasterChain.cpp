/*
  ==============================================================================

    MasterChain.cpp

  ==============================================================================
*/

#include "MasterChain.h"

//==============================================================================

MasterChain::MasterChain()
{
    masterHighPass.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    crossover.SetMonoLowBand(true);
}

void MasterChain::Prepare(double sr, int blockSize, int numChannels)
{
    juce::dsp::ProcessSpec spec;
        spec.sampleRate       = sr;
        spec.maximumBlockSize = (juce::uint32) blockSize;
        spec.numChannels      = (juce::uint32) juce::jmax(1, numChannels);

        masterHighPass.prepare(spec);
        masterHighPass.setCutoffFrequency(masterHpHz);

        crossover.Prepare(sr, blockSize, numChannels);
        haas     .Prepare(sr, blockSize, numChannels);
        limiter  .Prepare(sr, blockSize, numChannels);
}

void MasterChain::Reset()
{
    masterHighPass.reset();
    crossover     .Reset();
    haas          .Reset();
    limiter       .Reset();
}

void MasterChain::Process(juce::AudioBuffer<float> &buffer)
{
    if (widthParam == nullptr || monoCrossoverHzParam == nullptr)
        return;

    juce::dsp::AudioBlock<float>              block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    masterHighPass.process(ctx);

    crossover.SetCrossoverFrequency(*monoCrossoverHzParam);
    haas     .SetWidth(*widthParam);
    limiter  .SetEnabled((limiterOnParam != nullptr) && (*limiterOnParam > 0.5f));
    limiter  .SetCeiling((limiterCeilingDbParam != nullptr) ? limiterCeilingDbParam->load() : 0.0f);

    crossover.Process(buffer);

    haas.Process(crossover.HighBand());

    auto       &low      = crossover.LowBand();
    auto       &high     = crossover.HighBand();
    const int   numCh    = juce::jmin(buffer.getNumChannels()
                                      , juce::jmin(low.getNumChannels(), high.getNumChannels()));
    const int   numSamps = buffer.getNumSamples();

    for (int ch = 0; ch < numCh; ++ch)
    {
        auto       *dst = buffer.getWritePointer(ch);
        const auto *lo  = low .getReadPointer(ch);
        const auto *hi  = high.getReadPointer(ch);
        
        for (int i = 0; i < numSamps; ++i)
            dst[i] = lo[i] + hi[i];
    }

    limiter.Process(buffer);
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
