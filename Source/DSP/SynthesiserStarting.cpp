/*
  ==============================================================================

    SynthesiserStarting.cpp
    Created: 26 Dec 2020 5:24:06pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "SynthesiserStarting.h"



MySynthVoice::MySynthVoice() : playing(false), ending(false),
                               freq(0.0f), vel(0.0f), pitchBend(0.0f),
                               shiftHz(1.0f), previousPitchWheelValue(0.0f),
                               pitchBendSemitones(12.0f), pitchBendUpSemitones(12.0f),
                               pitchBendDownSemitones(12.0f), lastRecievedPitchWheelValue(0.0f),
                               filterSample(0.0f), masterGain(0.0f), sampleRate(44100.0f),
                               samplesPerBlock(0) {}

void MySynthVoice::init(float SR, int blockSize)
{
    // Set Master Sample Rate
    sampleRate      = SR;
    samplesPerBlock = blockSize;
    
    //
    // Sets sampleRates here
    //
    
    // Main and Sub Osc
    wtSine.setSampleRate  (sampleRate);
    wtSaw.setSampleRate   (sampleRate);
    wtSpike.setSampleRate (sampleRate);
    subOsc.setSampleRate  (sampleRate);
    env.setSampleRate     (sampleRate);
    //env.reset();
    
    // Modifiers
    ringMod.setSampleRate   (sampleRate);
    freqShift.setSampleRate (sampleRate);
    sAndH.setSampleRate     (sampleRate);
    
    // Filters
    twoPoleLPF.setSampleRate         (sampleRate);
    fourPoleLPF.setSampleRate        (sampleRate);
    eightPoleLPF.setSampleRate       (sampleRate);
    notchFilter.setSampleRate        (sampleRate);
    filtEnv.setSampleRate            (sampleRate);
    //filtEnv.reset();
    filtLFOClickingEnv.setSampleRate (sampleRate);
    //filtLFOClickingEnv.reset();
    
    
    // LFOs
    filterLFO.setSampleRate (sampleRate);
    
    //
    // Populates Wavetables
    //
    
    // Oscillators
    wtSine.populateWavetable();
    wtSaw.populateWavetable();
    wtSpike.populateWavetable();
    subOsc.populateWavetable();
    
    // LFOs
    filterLFO.populateWavetable();
    
    
    //
    // Value smoothing
    //
    
    setPortamentoTime(sampleRate, 0.02f);
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
    mainOscShape.setSize (1, 1024);
    subOscShape.setSize  (1, 1024);
    lfoOscShape.setSize  (1, 1024);
}


//
// Parameter Pointers Setup
//

// Main Oscs
void MySynthVoice::setOscParamPointers(std::atomic<float>* oscMorphIn, std::atomic<float>* subOscMorphIn,
                                       std::atomic<float>* subOscGainIn, std::atomic<float>* subOctaveIn)
{
    oscillatorMorph = oscMorphIn;
    subOscMorph     = subOscMorphIn;
    subGain         = subOscGainIn;
    subOctave       = subOctaveIn;
}

void MySynthVoice::setAmpADSRParamPointers(std::atomic<float>* attack, std::atomic<float>* decay,
                                           std::atomic<float>* sustain, std::atomic<float>* release)
{
    ampAttack  = attack;
    ampDecay   = decay;
    ampSustain = sustain;
    ampRelease = release;
}

void MySynthVoice::setDistParamPointers(std::atomic<float>* foldDistIn)
{
    foldbackDistortion = foldDistIn;
}

// Modifiers
void MySynthVoice::setRingModParamPointers(std::atomic<float>* ringPitch, std::atomic<float>* ringTone,
                                           std::atomic<float>* mix)
{
    ringModPitch = ringPitch;
    ringModTone  = ringTone;
    ringMix      = mix;
}

void MySynthVoice::setFreqShiftParamPointers(std::atomic<float>* shiftPitch, std::atomic<float>* mix)
{
    freqShiftPitch  = shiftPitch;
    freqShiftMixVal = mix;
}

void MySynthVoice::setSampleAndHoldParamPointers(std::atomic<float>* pitch, std::atomic<float>* mix)
{
    sAndHPitch  = pitch;
    sAndHMixVal = mix;
}

// Filters
void MySynthVoice::setFilterParamPointers(std::atomic<float>* cutoff, std::atomic<float>* res,
                                          std::atomic<float>* type)
{
    filterCutoffFreq = cutoff;
    filterResonance  = res;
    filterSelector   = type;
}

void MySynthVoice::setFilterADSRParamPointers(std::atomic<float>* attack, std::atomic<float>* decay,
                                              std::atomic<float>* sustain, std::atomic<float>* release,
                                              std::atomic<float>* amtCO, std::atomic<float>* amtRes )
{
    filterAttack           = attack;
    filterDecay            = decay;
    filterSustain          = sustain;
    filterRelease          = release;
    filterADSRCutOffAmount = amtCO;
    filterADSRResAmount    = amtRes;
}

void MySynthVoice::setFilterLFOParamPointers(std::atomic<float>* freq, std::atomic<float>* amount,
                                             std::atomic<float>* shape)
{
    filtLFOFreq  = freq;
    filtLFOAmt   = amount;
    filtLFOShape = shape;
}

void MySynthVoice::setPortamentoParamPointers(std::atomic<float>* portaTime)
{
    portamentoAmount = portaTime;
}

void MySynthVoice::setMasterGainParamPointers(std::atomic<float>* gainAmt)
{
    masterGainControl = gainAmt;
}

/*
void MySynthVoice::setPlayheadInfo(AudioPlayHead::CurrentPositionInfo& playhead)
{
    if (hostBPM != (float)playhead.bpm)
    {
        hostBPM = (float)playhead.bpm;
        twoPoleLPF.setPlayheadInfo   (playhead);
        fourPoleLPF.setPlayheadInfo  (playhead);
        eightPoleLPF.setPlayheadInfo (playhead);
        notchFilter.setPlayheadInfo  (playhead);
    }
}
*/


//
// Pitch Wheel methods:
//

/// synth class automatically sends newPitchWheelValue from its render block
void MySynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    if (previousPitchWheelValue != newPitchWheelValue)
    {
        previousPitchWheelValue = newPitchWheelValue;
        
        setPitchBend(newPitchWheelValue);
        
        shiftHz = calcShiftHz( pitchBendCents() );
    }
}

/// Pitch wheel position to pitchBend up or down
void MySynthVoice::setPitchBend(int pitchWheelPos)
{
    if (pitchWheelPos > 8192)
        {
            // shifting up
            pitchBend = float(pitchWheelPos - 8192) / (16383 - 8192);
        }
        else
        {
            // shifting down
            pitchBend = float(8192 - pitchWheelPos) / -8192;   // negative number
        }
}

/// calculates pitch wheel's shift in hz
float MySynthVoice::calcShiftHz(float centsOffset)
{
    return std::powf(2.0f, centsOffset / 1200.0f);
}

/// maps pitchwheel min/max positions to bend in cents as a function of pitchBend
float MySynthVoice::pitchBendCents()
{
    if (pitchBend >= 0.0f)
    {
        // shifting up
        return pitchBend * pitchBendUpSemitones * 100;
    }
    else
    {
        // shifting down
        return pitchBend * pitchBendDownSemitones * 100;
    }
}

/// Updates the number of semitones the pitchWheel will bend
void MySynthVoice::updatePitchBendRange(float newRange)
{
    pitchBendUpSemitones   = newRange;
    pitchBendDownSemitones = newRange;
}

//
// ADSR Values
//

// Main Osc ADSR
void MySynthVoice::setAmpADSRValues()
{
    envParams.attack  = *ampAttack;     // time (sec)
    envParams.decay   = *ampDecay;      // time (sec)
    envParams.sustain = *ampSustain;    // amplitude 0.0f to 1.0f
    envParams.release = *ampRelease;    // time (sec)
    
    env.setParameters(envParams);
}

/// Sets ADSR values for filter
void MySynthVoice::setFilterADSRValues()
{
    ADSR::Parameters filtEnvParams;
    
    filtEnvParams.attack  = *filterAttack;
    filtEnvParams.decay   = *filterDecay;
    filtEnvParams.sustain = *filterSustain;
    filtEnvParams.release = *filterRelease;
    
    filtEnv.setParameters(filtEnvParams);
}

/// Applies ADSR to LFO to avoid clicking
void MySynthVoice::setFiltLFOClickValues()
{
    ADSR::Parameters filtLFOClickParams;
    
    filtLFOClickParams.attack  = 0.02f;
    filtLFOClickParams.decay   = 0.5f;
    filtLFOClickParams.sustain = 1.0f;
    filtLFOClickParams.release = 0.02f;
    
    filtLFOClickingEnv.setParameters(filtLFOClickParams);
}

/// Sets up the portamentoTime
void MySynthVoice::setPortamentoTime(float SR, float portaTime)
{
    portamento.reset(SR, portaTime);
}


//--------------------------------------------------------------------------

void MySynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int currentPitchWheelPosition)
{
    playing = true;
    ending  = false;
    
    setPortamentoTime(sampleRate, *portamentoAmount);
    
    // Sets Amp ADSR for each note
    setAmpADSRValues();
    setFilterADSRValues();
    
    // Set Sub Octave
    incrementDenominator = subOscParamControl.subOctaveSelector(subOctave);
    
    // This just ensures the wheel position is set before the freq is calculated.
    // The actual pitch bend frequency multiplication happens in the render block.
    setPitchBend( currentPitchWheelPosition );
    
    // Converts incoming MIDI note to frequency
    freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
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

//--------------------------------------------------------------------------

// The Main DSP Block
void MySynthVoice::renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    const auto levels = computeBlockLevels();
    populateVisualBuffers(levels);

    if (! playing)
        return;

    prepareDspForBlock();

    // Per-sample change-detection state for increment updates.
    float previousFinalFreq    = 0.0f;
    float prevRingModPitch     = 0.0f;
    float prevFreqShiftPitch   = 0.0f;
    float prevSAndHPitch       = 0.0f;
    int   previousIncrementDenom = 1;

    for (int sampleIndex = startSample; sampleIndex < startSample + numSamples; ++sampleIndex)
    {
        // Final playback frequency (portamento + pitch bend).
        const float portaFreq = portamento.getNextValue();
        const float finalFreq = portaFreq * shiftHz;

        // Update wavetable increments only when the playback freq changes.
        if (previousFinalFreq != finalFreq)
        {
            wtSine.setIncrement  (finalFreq);
            wtSaw.setIncrement   (finalFreq);
            wtSpike.setIncrement (finalFreq);
            subOsc.setIncrement  (finalFreq, incrementDenominator);

            previousFinalFreq = finalFreq;
        }

        if (previousIncrementDenom != incrementDenominator)
        {
            subOsc.setIncrement (finalFreq, incrementDenominator);
            previousIncrementDenom = incrementDenominator;
        }

        // Update modifier-osc increments when freq or their pitch params change.
        if (previousFinalFreq != finalFreq || prevRingModPitch != *ringModPitch)
        {
            ringMod.modFreq (finalFreq, ringModPitch);
            prevRingModPitch = *ringModPitch;
        }

        if (previousFinalFreq != finalFreq || prevFreqShiftPitch != *freqShiftPitch)
        {
            freqShift.modFreq (finalFreq, freqShiftPitch);
            prevFreqShiftPitch = *freqShiftPitch;
        }

        if (previousFinalFreq != finalFreq || prevSAndHPitch != *sAndHPitch)
        {
            sAndH.modFreq (finalFreq, sAndHPitch);
            prevFreqShiftPitch = *sAndHPitch;   // TODO: check prevFreqShiftPitch should be prevSAndHPitch
        }

        // Envelopes (advance once per sample).
        const float envVal        = env.getNextSample();
        const float filtEnvVal    = filtEnv.getNextSample();
        const float filtLFOEnvVal = filtLFOClickingEnv.getNextSample();

        // DSP pipeline.
        const float mainSample     = processMainOscSample (envVal, levels);
        const float modifiedSample = processModifierChain (mainSample, envVal);
        const float subSample      = processSubOscSample  (envVal, levels);
        const float mixedSample    = (modifiedSample + subSample) * 0.75f;
        const float filteredSample = processFilterChain   (mixedSample, filtEnvVal, filtLFOEnvVal, levels);
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

//
// renderNextBlock helpers
//

MySynthVoice::BlockLevels MySynthVoice::computeBlockLevels()
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

void MySynthVoice::populateVisualBuffers(const BlockLevels& levels)
{
    mainOscShape.clear();
    subOscShape.clear();
    lfoOscShape.clear();

    populateShape (mainOscShape, levels.mainSin, levels.mainSpike,  levels.mainSaw, false);
    populateShape (subOscShape,  levels.subSin,  levels.subSquare,  levels.subSaw,  true);
    populateShape (lfoOscShape,  levels.lfoSin,  levels.lfoSquare,  levels.lfoSaw,  true);
}

void MySynthVoice::prepareDspForBlock()
{
    // Block-level mod/oscillator setup.
    ringMod.setRingToneSlider (ringModTone);
    freqShift.oscMorph        (oscillatorMorph);
    freqShift.modFreq         (freq, freqShiftPitch);
    filterLFO.setIncrement    (*filtLFOFreq, 1.0f);

    // Smoothed-value targets for the block.
    foldbackDistortionSmooth.setTargetValue (*foldbackDistortion);
    subGainSmooth.setTargetValue            (*subGain);
    ringMixSmooth.setTargetValue            (*ringMix);
    freqShiftMixValSmooth.setTargetValue    (*freqShiftMixVal);
    sAndHMixValSmooth.setTargetValue        (*sAndHMixVal);
    masterGainControlSmooth.setTargetValue  (*masterGainControl);
    velocitySmooth.setTargetValue           (vel);
    filterCutoffFreqSmooth.setTargetValue   (*filterCutoffFreq);
}

float MySynthVoice::processMainOscSample(float envVal, const BlockLevels& levels)
{
    const float sinSample   = wtSine.process()  * levels.mainSin   * envVal;
    const float spikeSample = wtSpike.process() * levels.mainSpike * envVal;
    const float sawSample   = wtSaw.process()   * levels.mainSaw   * envVal;

    // Sum of three shapes scaled so simultaneous shapes don't clip, then foldback.
    const float oscSample = (sinSample + spikeSample + sawSample) * 0.5f;
    const float foldback  = foldbackDistortionSmooth.getNextValue();

    return std::sin (oscSample * foldback);
}

float MySynthVoice::processModifierChain(float input, float envVal)
{
    // Ring Modulation
    const float ringSample = input * ringMod.process() * envVal;
    const float oscRing    = ringModMix.dryWetMix (input, ringSample, ringMixSmooth.getNextValue());

    // Frequency Shifter
    const float freqShiftSample = freqShift.process() * envVal;
    const float oscShift        = freqShiftMix.dryWetMix (oscRing, freqShiftSample, freqShiftMixValSmooth.getNextValue());

    // Sample and Hold
    const float sandhSample = sAndH.processSH (oscShift) * envVal;
    return sAndHMix.dryWetMix (oscShift, sandhSample, sAndHMixValSmooth.getNextValue());
}

float MySynthVoice::processSubOscSample(float envVal, const BlockLevels& levels)
{
    return subOsc.process (levels.subSin, levels.subSquare, levels.subSaw)
         * subGainSmooth.getNextValue()
         * envVal;
}

float MySynthVoice::processFilterChain(float input, float filtEnvVal, float filtLFOEnvVal, const BlockLevels& levels)
{
    const float filtLFOSample      = filterLFO.process (levels.lfoSin, levels.lfoSquare, levels.lfoSaw) * filtLFOEnvVal;
    const float filtCutoffSmoothed = filterCutoffFreqSmooth.getNextValue();

    switch ((int) *filterSelector)
    {
        case 1:
            filterSample = fourPoleLPF.processFilter  (freq, filtCutoffSmoothed, filterResonance, input, filtEnvVal,
                                                       filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
            break;
        case 2:
            filterSample = eightPoleLPF.processFilter (freq, filtCutoffSmoothed, filterResonance, input, filtEnvVal,
                                                       filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
            break;
        case 3:
            filterSample = notchFilter.processFilter  (freq, filtCutoffSmoothed, filterResonance, input, filtEnvVal,
                                                       filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
            break;
        case 0:
        default:
            filterSample = twoPoleLPF.processFilter   (freq, filtCutoffSmoothed, filterResonance, input, filtEnvVal,
                                                       filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
            break;
    }

    return filterSample;
}

bool MySynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast<MySynthSound*> (sound) != nullptr;
}

/// Returns the buffer of the main oscillator shape
AudioBuffer<float> MySynthVoice::oscVisualBuffer()
{
    return mainOscShape;
}

/// Returns the buffer of the sub oscillator shape
AudioBuffer<float> MySynthVoice::subVisualBuffer()
{
    return subOscShape;
}

AudioBuffer<float> MySynthVoice::lfoVisualBuffer()
{
    return lfoOscShape;
}





/// Populates shape buffer with morphed wave values
void MySynthVoice::populateShape(AudioBuffer<float>& buf, float sin, float spikeSqr, float saw, bool isSubOsc)
{
    for (int i=0; i<buf.getNumSamples(); i++)
    {
        float sinVal = sin * wtSine.getWavetableSampleValue(i);
        float sawVal = saw * wtSaw.getWavetableSampleValue(i);
        float centerWaveVal;
        
        isSubOsc == true ? centerWaveVal = spikeSqr * subOsc.getSquareWavetableValue(i) : centerWaveVal = spikeSqr * wtSpike.getWavetableSampleValue(i);
        
        float sampleVal = sinVal + centerWaveVal + sawVal;
        
        buf.addSample(0, i, sampleVal);
    }
}

