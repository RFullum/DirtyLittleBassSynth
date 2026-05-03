/*
  ==============================================================================

    BassSynthVoice.cpp
    Created: 26 Dec 2020 5:24:06pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "BassSynthVoice.h"



BassSynthVoice::BassSynthVoice()
: playing(false)
, ending(false)
, freq(0.0f)
, vel(0.0f)
, pitchBend(0.0f)
, shiftHz(1.0f)
, previousPitchWheelValue(0.0f)
, pitchBendSemitones(12.0f)
, pitchBendUpSemitones(12.0f)
, pitchBendDownSemitones(12.0f)
, lastReceivedPitchWheelValue(0.0f)
, masterGain(0.0f)
, sampleRate(44100.0f)
, samplesPerBlock(0)
{}

void BassSynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    playing = true;
    ending  = false;

    SetPortamentoTime(sampleRate, *portamentoAmount);
    SetAmpADSRValues();
    SetFilterADSRValues();

    incrementDenominator = subOscParamControl.SubOctaveSelector(subOctave);

    // Set the wheel position before the freq is calculated; the actual pitch-bend
    // multiplication happens in renderNextBlock.
    SetPitchBend(currentPitchWheelPosition);

    freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    portamento.setTargetValue(freq);

    env.noteOn();
    filtEnv.noteOn();
    filtLFOClickingEnv.noteOn();

    vel = velocity;
}

void BassSynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        env.noteOff();
        filtEnv.noteOff();
        filtLFOClickingEnv.noteOff();
        ending = true;
    }
    else
    {
        clearCurrentNote();
        playing = false;
    }
}

void BassSynthVoice::renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples)
{
    if (! playing)
        return;

    const auto levels = ComputeBlockLevels();
    PrepareDspForBlock(levels);

    // ===== PASS 1: Base-rate per-sample state =====
    // Advance the wavetable phasors, env, ADSRs, and parameter smoothers at the
    // base sample rate. Build the pre-foldback main-osc signal into a scratch
    // buffer that pass 2 will feed into the oversampling stage.
    auto *preFoldData = preFoldbackBuf.getWritePointer(0);

    float previousFinalFreq      = 0.0f;
    float prevRingModPitch       = 0.0f;
    float prevFreqShiftPitch     = 0.0f;
    float prevSAndHPitch         = 0.0f;
    int   previousIncrementDenom = 1;

    for (int i = 0; i < numSamples; ++i)
    {
        const float portaFreq = portamento.getNextValue();
        const float finalFreq = portaFreq * shiftHz;

        if (previousFinalFreq != finalFreq)
        {
            wtSine .SetIncrement(finalFreq);
            wtSaw  .SetIncrement(finalFreq);
            wtSpike.SetIncrement(finalFreq);
            subOsc .SetIncrement(finalFreq, incrementDenominator);

            previousFinalFreq = finalFreq;
        }

        if (previousIncrementDenom != incrementDenominator)
        {
            subOsc.SetIncrement(finalFreq, incrementDenominator);
            previousIncrementDenom = incrementDenominator;
        }

        if (previousFinalFreq != finalFreq || prevRingModPitch != ringModPitchVal)
        {
            ringMod.ModFreq(finalFreq, ringModPitchVal);
            prevRingModPitch = ringModPitchVal;
        }

        if (previousFinalFreq != finalFreq || prevFreqShiftPitch != freqShiftPitchVal)
        {
            freqShift.ModFreq(finalFreq, freqShiftPitchVal);
            prevFreqShiftPitch = freqShiftPitchVal;
        }

        if (previousFinalFreq != finalFreq || prevSAndHPitch != sAndHPitchVal)
        {
            sAndH.ModFreq(finalFreq, sAndHPitchVal);
            prevSAndHPitch = sAndHPitchVal;
        }

        // Advance envs / smoothers once per base sample, cache for pass 2.
        envValsCache       [(size_t) i] = env               .getNextSample();
        filtEnvValsCache   [(size_t) i] = filtEnv           .getNextSample();
        filtLFOEnvValsCache[(size_t) i] = filtLFOClickingEnv.getNextSample();
        foldbackCache      [(size_t) i] = foldbackDistortionSmooth.getNextValue();
        ringMixCache       [(size_t) i] = ringMixSmooth           .getNextValue();
        freqShiftMixCache  [(size_t) i] = freqShiftMixValSmooth   .getNextValue();
        sAndHMixCache      [(size_t) i] = sAndHMixValSmooth       .getNextValue();

        // Pre-foldback main osc: mipmap-band-limited shape mix scaled by env.
        const float envVal      = envValsCache[(size_t) i];
        const float sinSample   = wtSine .Process() * levels.mainSin   * envVal;
        const float spikeSample = wtSpike.Process() * levels.mainSpike * envVal;
        const float sawSample   = wtSaw  .Process() * levels.mainSaw   * envVal;

        // 0.5 prevents two summed shapes from clipping; foldback applied at OSR.
        preFoldData[i] = (sinSample + spikeSample + sawSample) * 0.5f;
    }

    // ===== PASS 2: 4× oversampled foldback + modifier chain =====
    juce::dsp::AudioBlock<float> baseBlock(preFoldbackBuf.getArrayOfWritePointers()
                                           , 1
                                           , (size_t)numSamples);

    auto upBlock = oversampling->processSamplesUp(baseBlock);

    auto       *upData     = upBlock.getChannelPointer(0);
    const int   upSamples  = (int) upBlock.getNumSamples();

    for (int i = 0; i < upSamples; ++i)
    {
        const int   baseIdx     = i / oversamplingFactor;
        const float envVal      = envValsCache    [(size_t) baseIdx];
        const float foldbackAmt = foldbackCache   [(size_t) baseIdx];
        const float ringMix     = ringMixCache    [(size_t) baseIdx];
        const float freqMix     = freqShiftMixCache[(size_t) baseIdx];
        const float sAndHMix    = sAndHMixCache   [(size_t) baseIdx];

        // Foldback is the most aggressive non-linearity; running it at 4× SR
        // keeps the harmonics it creates above audible Nyquist.
        float s = std::sin(upData[i] * foldbackAmt);

        // Modifier chain (ring mod → freq shift → sample-and-hold), each at OSR.
        const float ringSample = s * ringMod.Process() * envVal;
        const float oscRing    = DryWetMix(s, ringSample, ringMix);

        const float freqShiftSample = freqShift.Process() * envVal;
        const float oscShift        = DryWetMix(oscRing, freqShiftSample, freqMix);

        const float sandhSample = sAndH.ProcessSH(oscShift) * envVal;
        upData[i] = DryWetMix(oscShift, sandhSample, sAndHMix);
    }

    oversampling->processSamplesDown(baseBlock);

    // ===== PASS 3: Base-rate sub osc + filter + master gain =====
    const auto *postModData = preFoldbackBuf.getReadPointer(0);

    for (int i = 0; i < numSamples; ++i)
    {
        const float modifiedSample = postModData[i];
        const float envVal         = envValsCache       [(size_t) i];
        const float filtEnvVal     = filtEnvValsCache   [(size_t) i];
        const float filtLFOEnvVal  = filtLFOEnvValsCache[(size_t) i];

        const float subSample      = ProcessSubOscSample(envVal, levels);
        const float mixedSample    = (modifiedSample + subSample) * 0.75f;
        const float filteredSample = ProcessFilterChain(mixedSample, filtEnvVal, filtLFOEnvVal, levels);
        const float outputSample   = filteredSample
                                   * masterGainControlSmooth.getNextValue()
                                   * velocitySmooth.getNextValue();

        for (int chan = 0; chan < outputBuffer.getNumChannels(); ++chan)
            outputBuffer.addSample(chan, startSample + i, outputSample);

        if (ending && envVal < 0.001f)
        {
            env               .reset();
            filtEnv           .reset();
            filtLFOClickingEnv.reset();
            playing = false;
        }
    }
}

void BassSynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    if (previousPitchWheelValue != newPitchWheelValue)
    {
        previousPitchWheelValue = newPitchWheelValue;
        SetPitchBend(newPitchWheelValue);
        shiftHz = CalcShiftHz(PitchBendCents());
    }
}

bool BassSynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<BassSynthSound*>(sound) != nullptr;
}

void BassSynthVoice::Init(float SR, int blockSize)
{
    sampleRate      = SR;
    samplesPerBlock = blockSize;

    wtSine .SetSampleRate(sampleRate);
    wtSaw  .SetSampleRate(sampleRate);
    wtSpike.SetSampleRate(sampleRate);
    subOsc .SetSampleRate(sampleRate);
    env    .setSampleRate(sampleRate);

    // Modifiers run inside the 4× oversampled section, so their internal
    // oscillators are clocked at 4× base. ModFreq() then computes the right
    // increment to produce the desired modulator frequency in real time.
    const float oversampledRate = sampleRate * (float) oversamplingFactor;
    ringMod  .SetSampleRate(oversampledRate);
    freqShift.SetSampleRate(oversampledRate);
    sAndH    .SetSampleRate(oversampledRate);

    twoPoleLPF        .SetSampleRate(sampleRate);
    fourPoleLPF       .SetSampleRate(sampleRate);
    eightPoleLPF      .SetSampleRate(sampleRate);
    notchFilter       .SetSampleRate(sampleRate);
    filtEnv           .setSampleRate(sampleRate);
    filtLFOClickingEnv.setSampleRate(sampleRate);

    filterLFO.SetSampleRate(sampleRate);

    wtSine.PopulateWavetable();
    wtSaw.PopulateWavetable();
    wtSpike.PopulateWavetable();
    subOsc.PopulateWavetable();

    filterLFO.PopulateWavetable();

    SetPortamentoTime(sampleRate, 0.02f);
    portamento.setCurrentAndTargetValue(0.0f);

    subGainSmooth.reset(sampleRate, 0.01f);
    subGainSmooth.setCurrentAndTargetValue(0.0f);

    foldbackDistortionSmooth.reset(sampleRate, 0.01f);
    foldbackDistortionSmooth.setCurrentAndTargetValue(0.0f);

    ringMixSmooth.reset(sampleRate, 0.01f);
    ringMixSmooth.setCurrentAndTargetValue(0.0f);

    freqShiftMixValSmooth.reset(sampleRate, 0.01f);
    freqShiftMixValSmooth.setCurrentAndTargetValue(0.0f);

    sAndHMixValSmooth.reset(sampleRate, 0.01f);
    sAndHMixValSmooth.setCurrentAndTargetValue(0.0f);

    filterCutoffFreqSmooth.reset(sampleRate, 0.01f);
    filterCutoffFreqSmooth.setCurrentAndTargetValue(0.0f);

    masterGainControlSmooth.reset(sampleRate, 0.01f);
    masterGainControlSmooth.setCurrentAndTargetValue(1.0f);

    velocitySmooth.reset(sampleRate, 0.01f);
    velocitySmooth.setCurrentAndTargetValue(1.0f);

    // === Oversampling stage ===
    // Polyphase IIR halfband filters: minimum-phase, low latency. Integer-sample
    // latency makes host compensation clean.
    oversampling = std::make_unique<juce::dsp::Oversampling<float>>(1   /*numChannels*/
                                                                    , (size_t)oversamplingFactorLog2    /*factor (log2)*/
                                                                    , juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR    /*filterType*/
                                                                    , true  /*isMaxQuality*/
                                                                    , true);    /*useIntegerLatency */

    oversampling->initProcessing((size_t) blockSize);
    oversampling->reset();

    // Pre-allocate scratch sized for the host's reported max block size so the
    // audio thread never reallocates.
    preFoldbackBuf.setSize(1, blockSize, false, true, true);

    envValsCache       .assign((size_t) blockSize, 0.0f);
    filtEnvValsCache   .assign((size_t) blockSize, 0.0f);
    filtLFOEnvValsCache.assign((size_t) blockSize, 0.0f);
    foldbackCache      .assign((size_t) blockSize, 0.0f);
    ringMixCache       .assign((size_t) blockSize, 0.0f);
    freqShiftMixCache  .assign((size_t) blockSize, 0.0f);
    sAndHMixCache      .assign((size_t) blockSize, 0.0f);
}

int BassSynthVoice::GetOversamplingLatencyInSamples() const noexcept
{
    if (oversampling == nullptr)
        return 0;

    return (int) std::ceil(oversampling->getLatencyInSamples());
}

void BassSynthVoice::SetOscParamPointers(std::atomic<float>   *oscMorphIn
                                       , std::atomic<float> *subOscMorphIn
                                       , std::atomic<float> *subOscGainIn
                                       , std::atomic<float> *subOctaveIn)
{
    oscillatorMorph = oscMorphIn;
    subOscMorph     = subOscMorphIn;
    subGain         = subOscGainIn;
    subOctave       = subOctaveIn;
}

void BassSynthVoice::SetAmpADSRParamPointers(std::atomic<float>   *attack
                                           , std::atomic<float> *decay
                                           , std::atomic<float> *sustain
                                           , std::atomic<float> *release)
{
    ampAttack  = attack;
    ampDecay   = decay;
    ampSustain = sustain;
    ampRelease = release;
}

void BassSynthVoice::SetDistParamPointers(std::atomic<float> *foldDistIn)
{
    foldbackDistortion = foldDistIn;
}

void BassSynthVoice::SetRingModParamPointers(std::atomic<float>   *ringPitch, std::atomic<float> *ringTone, std::atomic<float> *mix)
{
    ringModPitch = ringPitch;
    ringModTone  = ringTone;
    ringMix      = mix;
}

void BassSynthVoice::SetFreqShiftParamPointers(std::atomic<float> *shiftPitch, std::atomic<float> *mix)
{
    freqShiftPitch  = shiftPitch;
    freqShiftMixVal = mix;
}

void BassSynthVoice::SetSampleAndHoldParamPointers(std::atomic<float>* pitch, std::atomic<float>* mix)
{
    sAndHPitch  = pitch;
    sAndHMixVal = mix;
}

void BassSynthVoice::SetFilterParamPointers(std::atomic<float> *cutoff, std::atomic<float> *res, std::atomic<float> *type)
{
    filterCutoffFreq = cutoff;
    filterResonance  = res;
    filterSelector   = type;
}

void BassSynthVoice::SetFilterADSRParamPointers(std::atomic<float>   *attack
                                              , std::atomic<float> *decay
                                              , std::atomic<float> *sustain
                                              , std::atomic<float> *release
                                              , std::atomic<float> *amtCO
                                              , std::atomic<float> *amtRes)
{
    filterAttack           = attack;
    filterDecay            = decay;
    filterSustain          = sustain;
    filterRelease          = release;
    filterADSRCutOffAmount = amtCO;
    filterADSRResAmount    = amtRes;
}

void BassSynthVoice::SetFilterLFOParamPointers(std::atomic<float> *freq, std::atomic<float> *amount, std::atomic<float> *shape)
{
    filtLFOFreq  = freq;
    filtLFOAmt   = amount;
    filtLFOShape = shape;
}

void BassSynthVoice::SetFilterLFOSyncParamPointers(std::atomic<float> *syncOn, std::atomic<float> *syncDivIndex)
{
    filtLFOSyncOn       = syncOn;
    filtLFOSyncDivIndex = syncDivIndex;
}

void BassSynthVoice::SetTempoSnapshot(const TempoSnapshot *snapshot)
{
    tempoSnapshotPtr = snapshot;
}

void BassSynthVoice::SetPortamentoParamPointers(std::atomic<float> *portaTime)
{
    portamentoAmount = portaTime;
}

void BassSynthVoice::SetMasterGainParamPointers(std::atomic<float> *gainAmt)
{
    masterGainControl = gainAmt;
}

void BassSynthVoice::SetAmpADSRValues()
{
    envParams.attack  = *ampAttack;     // sec
    envParams.decay   = *ampDecay;      // sec
    envParams.sustain = *ampSustain;    // amplitude 0..1
    envParams.release = *ampRelease;    // sec

    env.setParameters(envParams);
}

void BassSynthVoice::SetFilterADSRValues()
{
    juce::ADSR::Parameters filtEnvParams;

    filtEnvParams.attack  = *filterAttack;
    filtEnvParams.decay   = *filterDecay;
    filtEnvParams.sustain = *filterSustain;
    filtEnvParams.release = *filterRelease;

    filtEnv.setParameters(filtEnvParams);
}

/// Short ADSR applied to the filter LFO so it fades in/out instead of clicking.
void BassSynthVoice::SetFiltLFOClickValues()
{
    juce::ADSR::Parameters filtLFOClickParams;

    filtLFOClickParams.attack  = 0.02f;
    filtLFOClickParams.decay   = 0.5f;
    filtLFOClickParams.sustain = 1.0f;
    filtLFOClickParams.release = 0.02f;

    filtLFOClickingEnv.setParameters(filtLFOClickParams);
}

void BassSynthVoice::SetPortamentoTime(float SR, float portaTime)
{
    portamento.reset(SR, portaTime);
}

void BassSynthVoice::updatePitchBendRange(float newRange)
{
    pitchBendUpSemitones   = newRange;
    pitchBendDownSemitones = newRange;
}

BassSynthVoice::BlockLevels BassSynthVoice::ComputeBlockLevels()
{
    return {
        oscParamControl.SinMorphGain      (oscillatorMorph),
        oscParamControl.SpikeMorphGain    (oscillatorMorph),
        oscParamControl.SawMorphGain      (oscillatorMorph),
        subOscParamControl.SinSubGain     (subOscMorph),
        subOscParamControl.SquareSubGain  (subOscMorph),
        subOscParamControl.SawSubGain     (subOscMorph),
        filtLFOShapeControl.SinSubGain    (filtLFOShape),
        filtLFOShapeControl.SquareSubGain (filtLFOShape),
        filtLFOShapeControl.SawSubGain    (filtLFOShape)
    };
}

void BassSynthVoice::PrepareDspForBlock(const BlockLevels &levels)
{
    // Cache atomic-loaded params as plain floats so the per-sample loop never derefs
    // std::atomic<float>* on the audio thread.
    ringModPitchVal           = *ringModPitch;
    freqShiftPitchVal         = *freqShiftPitch;
    sAndHPitchVal             = *sAndHPitch;
    filterSelectorIndex       = (int)*filterSelector;
    filterResonanceVal        = *filterResonance;
    filterADSRCutOffAmountVal = *filterADSRCutOffAmount;
    filterADSRResAmountVal    = *filterADSRResAmount;
    filtLFOAmtVal             = *filtLFOAmt;

    switch (filterSelectorIndex)
    {
        case 1:  activeFilter = &fourPoleLPF;  break;
        case 2:  activeFilter = &eightPoleLPF; break;
        case 3:  activeFilter = &notchFilter;  break;
        case 0:  [[fallthrough]];
        default: activeFilter = &twoPoleLPF;   break;
    }

    ringMod  .SetRingToneSlider(*ringModTone);
    freqShift.OscMorph(levels.mainSin, levels.mainSpike, levels.mainSaw);
    freqShift.ModFreq(freq, freqShiftPitchVal);

    // === Filter LFO frequency / phase ===
    // FRQ mode: free-run at the slider's Hz value.
    // SYNC mode: derive Hz from host BPM × subdivision. When the transport is
    // playing, lock phase to ppqPosition so the LFO cycles align with the song
    // grid; when stopped, free-run at the synced equivalent rate.
    const bool lfoSyncOn = (filtLFOSyncOn != nullptr) && (*filtLFOSyncOn > 0.5f);

    float lfoFreqHz = *filtLFOFreq;

    if (lfoSyncOn && filtLFOSyncDivIndex != nullptr && tempoSnapshotPtr != nullptr)
    {
        const auto  tempo            = tempoSnapshotPtr->Read();
        const int   divIndex         = juce::jlimit(0, 11, (int)*filtLFOSyncDivIndex);
        const float subdivQuarters   = SubdivisionInQuarters(divIndex);
        const float beatsPerSecond   = tempo.bpm / 60.0f;

        lfoFreqHz = beatsPerSecond / juce::jmax(0.0001f, subdivQuarters);

        if (tempo.isPlaying)
        {
            const float phaseInCycles = tempo.ppqPosition / juce::jmax(0.0001f, subdivQuarters);
            const float wrapped       = phaseInCycles - std::floor(phaseInCycles);
            filterLFO.SetPhase(wrapped);
        }
    }

    filterLFO.SetIncrement(lfoFreqHz, 1.0f);

    foldbackDistortionSmooth.setTargetValue(*foldbackDistortion);
    subGainSmooth           .setTargetValue(*subGain);
    ringMixSmooth           .setTargetValue(*ringMix);
    freqShiftMixValSmooth   .setTargetValue(*freqShiftMixVal);
    sAndHMixValSmooth       .setTargetValue(*sAndHMixVal);
    masterGainControlSmooth .setTargetValue(*masterGainControl);
    velocitySmooth          .setTargetValue(vel);
    filterCutoffFreqSmooth  .setTargetValue(*filterCutoffFreq);
}

float BassSynthVoice::ProcessSubOscSample(float envVal, const BlockLevels &levels)
{
    return subOsc.Process(levels.subSin, levels.subSquare, levels.subSaw)
         * subGainSmooth.getNextValue()
         * envVal;
}

float BassSynthVoice::ProcessFilterChain(float input, float filtEnvVal, float filtLFOEnvVal, const BlockLevels &levels)
{
    const float filtLFOSample      = filterLFO.Process(levels.lfoSin, levels.lfoSquare, levels.lfoSaw) * filtLFOEnvVal;
    const float filtCutoffSmoothed = filterCutoffFreqSmooth.getNextValue();

    return activeFilter->ProcessFilter(freq
                                       , filtCutoffSmoothed
                                       , filterResonanceVal
                                       , input
                                       , filtEnvVal
                                       , filterADSRCutOffAmountVal
                                       , filterADSRResAmountVal
                                       , filtLFOSample
                                       , filtLFOAmtVal);
}

float BassSynthVoice::PitchBendCents()
{
    if (pitchBend >= 0.0f)
        return pitchBend * pitchBendUpSemitones * 100;
    else
        return pitchBend * pitchBendDownSemitones * 100;
}

float BassSynthVoice::CalcShiftHz(float centsOffset)
{
    return std::pow(2.0f, centsOffset / 1200.0f);
}

void BassSynthVoice::SetPitchBend(int pitchWheelPos)
{
    if (pitchWheelPos > 8192)
        pitchBend = float(pitchWheelPos - 8192) / (16383 - 8192);
    else
        pitchBend = float(8192 - pitchWheelPos) / -8192;
}
