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

void BassSynthVoice::Init(float SR, int blockSize)
{
    sampleRate      = SR;
    samplesPerBlock = blockSize;
    
    wtSine .SetSampleRate(sampleRate);
    wtSaw  .SetSampleRate(sampleRate);
    wtSpike.SetSampleRate(sampleRate);
    subOsc .SetSampleRate(sampleRate);
    env    .setSampleRate(sampleRate);
    
    ringMod  .SetSampleRate(sampleRate);
    freqShift.SetSampleRate(sampleRate);
    sAndH    .SetSampleRate(sampleRate);
    
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

void BassSynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    playing = true;
    ending  = false;

    SetPortamentoTime(sampleRate, *portamentoAmount);
    SetAmpADSRValues();
    SetFilterADSRValues();

    incrementDenominator = subOscParamControl.subOctaveSelector(subOctave);

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
    if (!playing)
        return;

    const auto levels = ComputeBlockLevels();
    PrepareDspForBlock(levels);

    // Per-sample change-detection state for increment updates.
    float previousFinalFreq      = 0.0f;
    float prevRingModPitch       = 0.0f;
    float prevFreqShiftPitch     = 0.0f;
    float prevSAndHPitch         = 0.0f;
    int   previousIncrementDenom = 1;

    for (int sampleIndex = startSample; sampleIndex < startSample + numSamples; ++sampleIndex)
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

        const float envVal        = env.getNextSample();
        const float filtEnvVal    = filtEnv.getNextSample();
        const float filtLFOEnvVal = filtLFOClickingEnv.getNextSample();

        const float mainSample     = ProcessMainOscSample(envVal, levels);
        const float modifiedSample = ProcessModifierChain(mainSample, envVal);
        const float subSample      = ProcessSubOscSample(envVal, levels);
        const float mixedSample    = (modifiedSample + subSample) * 0.75f;
        const float filteredSample = ProcessFilterChain(mixedSample, filtEnvVal, filtLFOEnvVal, levels);
        const float outputSample   = filteredSample
                                   * masterGainControlSmooth.getNextValue()
                                   * velocitySmooth.getNextValue();

        for (int chan = 0; chan < outputBuffer.getNumChannels(); ++chan)
            outputBuffer.addSample(chan, sampleIndex, outputSample);

        if (ending && envVal < 0.001f)
        {
            env.reset();
            filtEnv.reset();
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

void BassSynthVoice::updatePitchBendRange(float newRange)
{
    pitchBendUpSemitones   = newRange;
    pitchBendDownSemitones = newRange;
}

bool BassSynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<BassSynthSound*>(sound) != nullptr;
}

BassSynthVoice::BlockLevels BassSynthVoice::ComputeBlockLevels()
{
    return {
        oscParamControl.sinMorphGain      (oscillatorMorph),
        oscParamControl.spikeMorphGain    (oscillatorMorph),
        oscParamControl.sawMorphGain      (oscillatorMorph),
        subOscParamControl.sinSubGain     (subOscMorph),
        subOscParamControl.squareSubGain  (subOscMorph),
        subOscParamControl.sawSubGain     (subOscMorph),
        filtLFOShapeControl.sinSubGain    (filtLFOShape),
        filtLFOShapeControl.squareSubGain (filtLFOShape),
        filtLFOShapeControl.sawSubGain    (filtLFOShape)
    };
}

void BassSynthVoice::PrepareDspForBlock(const BlockLevels &levels)
{
    // Cache atomic-loaded params as plain floats so the per-sample loop never derefs
    // std::atomic<float>* on the audio thread.
    ringModPitchVal           = *ringModPitch;
    freqShiftPitchVal         = *freqShiftPitch;
    sAndHPitchVal             = *sAndHPitch;
    filterSelectorIndex       = (int) *filterSelector;
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
    filterLFO.SetIncrement(*filtLFOFreq, 1.0f);

    foldbackDistortionSmooth.setTargetValue(*foldbackDistortion);
    subGainSmooth           .setTargetValue(*subGain);
    ringMixSmooth           .setTargetValue(*ringMix);
    freqShiftMixValSmooth   .setTargetValue(*freqShiftMixVal);
    sAndHMixValSmooth       .setTargetValue(*sAndHMixVal);
    masterGainControlSmooth .setTargetValue(*masterGainControl);
    velocitySmooth          .setTargetValue(vel);
    filterCutoffFreqSmooth  .setTargetValue(*filterCutoffFreq);
}

float BassSynthVoice::ProcessMainOscSample(float envVal, const BlockLevels& levels)
{
    const float sinSample   = wtSine.Process()  * levels.mainSin   * envVal;
    const float spikeSample = wtSpike.Process() * levels.mainSpike * envVal;
    const float sawSample   = wtSaw.Process()   * levels.mainSaw   * envVal;

    // 0.5 prevents two summed shapes from clipping; foldback is sin-based wave folding.
    const float oscSample = (sinSample + spikeSample + sawSample) * 0.5f;
    const float foldback  = foldbackDistortionSmooth.getNextValue();

    return std::sin(oscSample * foldback);
}

float BassSynthVoice::ProcessModifierChain(float input, float envVal)
{
    const float ringSample = input * ringMod.Process() * envVal;
    const float oscRing    = DryWetMix(input, ringSample, ringMixSmooth.getNextValue());

    const float freqShiftSample = freqShift.Process() * envVal;
    const float oscShift        = DryWetMix(oscRing, freqShiftSample, freqShiftMixValSmooth.getNextValue());

    const float sandhSample = sAndH.ProcessSH(oscShift) * envVal;
    return DryWetMix(oscShift, sandhSample, sAndHMixValSmooth.getNextValue());
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

