/*
  ==============================================================================

    SynthesiserStarting.cpp
    Created: 26 Dec 2020 5:24:06pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "SynthesiserStarting.h"



MySynthVoice::MySynthVoice()
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
, lastRecievedPitchWheelValue(0.0f)
, filterSample(0.0f)
, masterGain(0.0f)
, sampleRate(44100.0f)
, samplesPerBlock(0)
{}

void MySynthVoice::Init(float SR, int blockSize)
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
    
    // WaveShape Drawing
    mainOscShape.setSize(1, 1024);
    subOscShape.setSize(1, 1024);
    lfoOscShape.setSize(1, 1024);
}

void MySynthVoice::SetOscParamPointers(std::atomic<float>   *oscMorphIn
                                       , std::atomic<float> *subOscMorphIn
                                       , std::atomic<float> *subOscGainIn
                                       , std::atomic<float> *subOctaveIn)
{
    oscillatorMorph = oscMorphIn;
    subOscMorph     = subOscMorphIn;
    subGain         = subOscGainIn;
    subOctave       = subOctaveIn;
}

void MySynthVoice::SetAmpADSRParamPointers(std::atomic<float>   *attack
                                           , std::atomic<float> *decay
                                           , std::atomic<float> *sustain
                                           , std::atomic<float> *release)
{
    ampAttack  = attack;
    ampDecay   = decay;
    ampSustain = sustain;
    ampRelease = release;
}

void MySynthVoice::SetDistParamPointers(std::atomic<float> *foldDistIn)
{
    foldbackDistortion = foldDistIn;
}

void MySynthVoice::SetRingModParamPointers(std::atomic<float>   *ringPitch, std::atomic<float> *ringTone, std::atomic<float> *mix)
{
    ringModPitch = ringPitch;
    ringModTone  = ringTone;
    ringMix      = mix;
}

void MySynthVoice::SetFreqShiftParamPointers(std::atomic<float> *shiftPitch, std::atomic<float> *mix)
{
    freqShiftPitch  = shiftPitch;
    freqShiftMixVal = mix;
}

void MySynthVoice::SetSampleAndHoldParamPointers(std::atomic<float>* pitch, std::atomic<float>* mix)
{
    sAndHPitch  = pitch;
    sAndHMixVal = mix;
}

void MySynthVoice::SetFilterParamPointers(std::atomic<float> *cutoff, std::atomic<float> *res, std::atomic<float> *type)
{
    filterCutoffFreq = cutoff;
    filterResonance  = res;
    filterSelector   = type;
}

void MySynthVoice::SetFilterADSRParamPointers(std::atomic<float>   *attack
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

void MySynthVoice::SetFilterLFOParamPointers(std::atomic<float> *freq, std::atomic<float> *amount, std::atomic<float> *shape)
{
    filtLFOFreq  = freq;
    filtLFOAmt   = amount;
    filtLFOShape = shape;
}

void MySynthVoice::SetPortamentoParamPointers(std::atomic<float> *portaTime)
{
    portamentoAmount = portaTime;
}

void MySynthVoice::SetMasterGainParamPointers(std::atomic<float> *gainAmt)
{
    masterGainControl = gainAmt;
}

/// synth class automatically sends newPitchWheelValue from its render block
void MySynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    if (previousPitchWheelValue != newPitchWheelValue)
    {
        previousPitchWheelValue = newPitchWheelValue;
        SetPitchBend(newPitchWheelValue);
        shiftHz = CalcShiftHz(PitchBendCents());
    }
}

/// Pitch wheel position to pitchBend up or down
void MySynthVoice::SetPitchBend(int pitchWheelPos)
{
    if (pitchWheelPos > 8192)
        pitchBend = float(pitchWheelPos - 8192) / (16383 - 8192);
    else
        pitchBend = float(8192 - pitchWheelPos) / -8192;
}

/// calculates pitch wheel's shift in hz
float MySynthVoice::CalcShiftHz(float centsOffset)
{
    return std::powf(2.0f, centsOffset / 1200.0f);
}

/// maps pitchwheel min/max positions to bend in cents as a function of pitchBend
float MySynthVoice::PitchBendCents()
{
    if (pitchBend >= 0.0f)
        return pitchBend * pitchBendUpSemitones * 100;
    else
        return pitchBend * pitchBendDownSemitones * 100;
}

/// Updates the number of semitones the pitchWheel will bend
void MySynthVoice::updatePitchBendRange(float newRange)
{
    pitchBendUpSemitones   = newRange;
    pitchBendDownSemitones = newRange;
}

void MySynthVoice::SetAmpADSRValues()
{
    envParams.attack  = *ampAttack;     // time (sec)
    envParams.decay   = *ampDecay;      // time (sec)
    envParams.sustain = *ampSustain;    // amplitude 0.0f to 1.0f
    envParams.release = *ampRelease;    // time (sec)
    
    env.setParameters(envParams);
}

/// Sets juce::ADSR values for filter
void MySynthVoice::SetFilterADSRValues()
{
    juce::ADSR::Parameters filtEnvParams;
    
    filtEnvParams.attack  = *filterAttack;
    filtEnvParams.decay   = *filterDecay;
    filtEnvParams.sustain = *filterSustain;
    filtEnvParams.release = *filterRelease;
    
    filtEnv.setParameters(filtEnvParams);
}

/// Applies juce::ADSR to LFO to avoid clicking
void MySynthVoice::SetFiltLFOClickValues()
{
    juce::ADSR::Parameters filtLFOClickParams;
    
    filtLFOClickParams.attack  = 0.02f;
    filtLFOClickParams.decay   = 0.5f;
    filtLFOClickParams.sustain = 1.0f;
    filtLFOClickParams.release = 0.02f;
    
    filtLFOClickingEnv.setParameters(filtLFOClickParams);
}

/// Sets up the portamentoTime
void MySynthVoice::SetPortamentoTime(float SR, float portaTime)
{
    portamento.reset(SR, portaTime);
}

void MySynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    playing = true;
    ending  = false;
    
    SetPortamentoTime(sampleRate, *portamentoAmount);
    
    // Sets Amp juce::ADSR for each note
    SetAmpADSRValues();
    SetFilterADSRValues();
    
    // Set Sub Octave
    incrementDenominator = subOscParamControl.subOctaveSelector(subOctave);
    
    // This just ensures the wheel position is set before the freq is calculated.
    // The actual pitch bend frequency multiplication happens in the render block.
    SetPitchBend(currentPitchWheelPosition);
    
    // Converts incoming MIDI note to frequency
    freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    portamento.setTargetValue(freq);
    
    // Envelopes
    // Amp envelope
    env.noteOn();   // Start envelope
    
    //Filter Envelope
    filtEnv.noteOn();
    filtLFOClickingEnv.noteOn();
    
    // Velocity
    vel = velocity;
}

void MySynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        // ends envelope over release time
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

void MySynthVoice::renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples)
{
    const auto levels = ComputeBlockLevels();
    PopulateVisualBuffers(levels);

    if (!playing)
        return;

    PrepareDspForBlock();

    // Per-sample change-detection state for increment updates.
    float previousFinalFreq      = 0.0f;
    float prevRingModPitch       = 0.0f;
    float prevFreqShiftPitch     = 0.0f;
    float prevSAndHPitch         = 0.0f;
    int   previousIncrementDenom = 1;

    for (int sampleIndex = startSample; sampleIndex < startSample + numSamples; ++sampleIndex)
    {
        // Final playback frequency (portamento + pitch bend).
        const float portaFreq = portamento.getNextValue();
        const float finalFreq = portaFreq * shiftHz;

        // Update wavetable increments only when the playback freq changes.
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

        // Update modifier-osc increments when freq or their pitch params change.
        if (previousFinalFreq != finalFreq || prevRingModPitch != *ringModPitch)
        {
            ringMod.ModFreq(finalFreq, ringModPitch);
            prevRingModPitch = *ringModPitch;
        }

        if (previousFinalFreq != finalFreq || prevFreqShiftPitch != *freqShiftPitch)
        {
            freqShift.ModFreq(finalFreq, freqShiftPitch);
            prevFreqShiftPitch = *freqShiftPitch;
        }

        if (previousFinalFreq != finalFreq || prevSAndHPitch != *sAndHPitch)
        {
            sAndH.ModFreq(finalFreq, sAndHPitch);
            prevFreqShiftPitch = *sAndHPitch;   // TODO: BUG ALERT! check prevFreqShiftPitch should be prevSAndHPitch
        }

        // Envelopes (advance once per sample).
        const float envVal        = env.getNextSample();
        const float filtEnvVal    = filtEnv.getNextSample();
        const float filtLFOEnvVal = filtLFOClickingEnv.getNextSample();

        // DSP pipeline.
        const float mainSample     = ProcessMainOscSample(envVal, levels);
        const float modifiedSample = ProcessModifierChain(mainSample, envVal);
        const float subSample      = ProcessSubOscSample(envVal, levels);
        const float mixedSample    = (modifiedSample + subSample) * 0.75f;
        const float filteredSample = ProcessFilterChain(mixedSample, filtEnvVal, filtLFOEnvVal, levels);
        const float outputSample   = filteredSample
                                   * masterGainControlSmooth.getNextValue()
                                   * velocitySmooth.getNextValue();

        for (int chan = 0; chan < outputBuffer.getNumChannels(); ++chan)
            outputBuffer.addSample (chan, sampleIndex, outputSample);

        // End-of-note: reset envelopes once they've decayed.
        if (ending && envVal < 0.001f)
        {
            env.reset();
            filtEnv.reset();
            filtLFOClickingEnv.reset();
            playing = false;
        }
    }
}

MySynthVoice::BlockLevels MySynthVoice::ComputeBlockLevels()
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

void MySynthVoice::PopulateVisualBuffers(const BlockLevels &levels)
{
    mainOscShape.clear();
    subOscShape.clear();
    lfoOscShape.clear();

    PopulateShape(mainOscShape, levels.mainSin, levels.mainSpike,  levels.mainSaw, false);
    PopulateShape(subOscShape,  levels.subSin,  levels.subSquare,  levels.subSaw,  true);
    PopulateShape(lfoOscShape,  levels.lfoSin,  levels.lfoSquare,  levels.lfoSaw,  true);
}

void MySynthVoice::PrepareDspForBlock()
{
    // Block-level mod/oscillator setup.
    ringMod.SetRingToneSlider(ringModTone);
    freqShift.OscMorph(oscillatorMorph);
    freqShift.ModFreq(freq, freqShiftPitch);
    filterLFO.SetIncrement(*filtLFOFreq, 1.0f);

    // Smoothed-value targets for the block.
    foldbackDistortionSmooth.setTargetValue(*foldbackDistortion);
    subGainSmooth           .setTargetValue(*subGain);
    ringMixSmooth           .setTargetValue(*ringMix);
    freqShiftMixValSmooth   .setTargetValue(*freqShiftMixVal);
    sAndHMixValSmooth       .setTargetValue(*sAndHMixVal);
    masterGainControlSmooth .setTargetValue(*masterGainControl);
    velocitySmooth          .setTargetValue(vel);
    filterCutoffFreqSmooth  .setTargetValue(*filterCutoffFreq);
}

float MySynthVoice::ProcessMainOscSample(float envVal, const BlockLevels& levels)
{
    const float sinSample   = wtSine.Process()  * levels.mainSin   * envVal;
    const float spikeSample = wtSpike.Process() * levels.mainSpike * envVal;
    const float sawSample   = wtSaw.Process()   * levels.mainSaw   * envVal;

    // Sum of three shapes scaled so simultaneous shapes don't clip, then foldback.
    const float oscSample = (sinSample + spikeSample + sawSample) * 0.5f;
    const float foldback  = foldbackDistortionSmooth.getNextValue();

    return std::sin (oscSample * foldback);
}

float MySynthVoice::ProcessModifierChain(float input, float envVal)
{
    const float ringSample = input * ringMod.Process() * envVal;
    const float oscRing    = ringModMix.dryWetMix(input, ringSample, ringMixSmooth.getNextValue());

    const float freqShiftSample = freqShift.Process() * envVal;
    const float oscShift        = freqShiftMix.dryWetMix(oscRing, freqShiftSample, freqShiftMixValSmooth.getNextValue());

    const float sandhSample = sAndH.ProcessSH(oscShift) * envVal;
    return sAndHMix.dryWetMix(oscShift, sandhSample, sAndHMixValSmooth.getNextValue());
}

float MySynthVoice::ProcessSubOscSample(float envVal, const BlockLevels &levels)
{
    return subOsc.Process(levels.subSin, levels.subSquare, levels.subSaw)
         * subGainSmooth.getNextValue()
         * envVal;
}

float MySynthVoice::ProcessFilterChain(float input, float filtEnvVal, float filtLFOEnvVal, const BlockLevels &levels)
{
    const float filtLFOSample      = filterLFO.Process(levels.lfoSin, levels.lfoSquare, levels.lfoSaw) * filtLFOEnvVal;
    const float filtCutoffSmoothed = filterCutoffFreqSmooth.getNextValue();

    switch ((int) *filterSelector)
    {
        case 1:
            filterSample = fourPoleLPF.ProcessFilter(freq
                                                     , filtCutoffSmoothed
                                                     , filterResonance
                                                     , input
                                                     , filtEnvVal
                                                     , filterADSRCutOffAmount
                                                     , filterADSRResAmount
                                                     , filtLFOSample
                                                     , filtLFOAmt);
            break;
        case 2:
            filterSample = eightPoleLPF.ProcessFilter(freq
                                                      , filtCutoffSmoothed
                                                      , filterResonance
                                                      , input
                                                      , filtEnvVal
                                                      , filterADSRCutOffAmount
                                                      , filterADSRResAmount
                                                      , filtLFOSample
                                                      , filtLFOAmt);
            break;
        case 3:
            filterSample = notchFilter.ProcessFilter(freq
                                                     , filtCutoffSmoothed
                                                     , filterResonance
                                                     , input
                                                     , filtEnvVal
                                                     , filterADSRCutOffAmount
                                                     , filterADSRResAmount
                                                     , filtLFOSample
                                                     , filtLFOAmt);
            break;
        case 0:     [[fallthrough]];
        default:
            filterSample = twoPoleLPF.ProcessFilter(freq
                                                    , filtCutoffSmoothed
                                                    , filterResonance
                                                    , input
                                                    , filtEnvVal
                                                    , filterADSRCutOffAmount
                                                    , filterADSRResAmount
                                                    , filtLFOSample
                                                    , filtLFOAmt);
            break;
    }

    return filterSample;
}

bool MySynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<MySynthSound*>(sound) != nullptr;
}

/// Returns the buffer of the main oscillator shape
juce::AudioBuffer<float> MySynthVoice::oscVisualBuffer()
{
    return mainOscShape;
}

/// Returns the buffer of the sub oscillator shape
juce::AudioBuffer<float> MySynthVoice::subVisualBuffer()
{
    return subOscShape;
}

juce::AudioBuffer<float> MySynthVoice::lfoVisualBuffer()
{
    return lfoOscShape;
}

/// Populates shape buffer with morphed wave values
void MySynthVoice::PopulateShape(juce::AudioBuffer<float> &buf, float sin, float spikeSqr, float saw, bool isSubOsc)
{
    for (int i=0; i<buf.getNumSamples(); i++)
    {
        float sinVal = sin * wtSine.GetWavetableSampleValue(i);
        float sawVal = saw * wtSaw.GetWavetableSampleValue(i);
        float centerWaveVal;
        
        centerWaveVal = isSubOsc == true
                            ? spikeSqr * subOsc.GetSquareWavetableValue(i)
                            : spikeSqr * wtSpike.GetWavetableSampleValue(i);
        
        float sampleVal = sinVal + centerWaveVal + sawVal;
        
        buf.addSample(0, i, sampleVal);
    }
}

