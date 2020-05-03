/*
  ==============================================================================

    MySynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

  ==============================================================================
*/

#pragma once

#include "Oscillators.h"
#include "Wavetable.h"
#include "SubOscillator.h"
#include "OscillatorParameterControls.h"
#include "DryWet.h"
#include "Modifiers.h"
#include "FilterSection.h"

// ===========================
// ===========================
// SOUND
class MySynthSound : public SynthesiserSound
{
public:
    bool appliesToNote      (int) override      { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override      { return true; }
};




// =================================
// =================================
// Synthesiser Voice - My synth code goes in here

/*!
 @class MySynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple MySynthVoice objects will be created by the Synthesiser so that it can be played polyphicially
 
 @namespace none
 @updated 2019-06-18
 */
class MySynthVoice : public SynthesiserVoice
{
public:
    MySynthVoice() {}
    
    void init(float SR)
    {
        // Set Master Sample Rate
        sampleRate = SR;
        
        //
        // Sets sampleRates here
        //
        
        // Main and Sub Osc
        wtSine.setSampleRate(sampleRate);
        wtSaw.setSampleRate(sampleRate);
        wtSpike.setSampleRate(sampleRate);
        subOsc.setSampleRate(sampleRate);
        env.setSampleRate(sampleRate);
        
        // Modifiers
        ringMod.setSampleRate(sampleRate);
        freqShift.setSampleRate(sampleRate);
        sAndH.setSampleRate(sampleRate);
        
        // Filters
        twoPoleLPF.setSampleRate(sampleRate);
        fourPoleLPF.setSampleRate(sampleRate);
        eightPoleLPF.setSampleRate(sampleRate);
        notchFilter.setSampleRate(sampleRate);
        filtEnv.setSampleRate(sampleRate);
        filtLFOClickingEnv.setSampleRate(sampleRate);
        
        // LFOs
        filterLFO.setSampleRate(sampleRate);
        
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
        
        masterGainControlSmooth.reset(sampleRate, 0.01f);
        masterGainControlSmooth.setCurrentAndTargetValue(1.0f);
        
    }
    
    //
    // Parameter Pointers Setup
    //
    
    // Main Oscs
    void setOscParamPointers(std::atomic<float>* oscMorphIn, std::atomic<float>* subOscMorphIn, std::atomic<float>* subOscGainIn, std::atomic<float>* subOctaveIn)
    {
        oscillatorMorph = oscMorphIn;
        subOscMorph = subOscMorphIn;
        subGain = subOscGainIn;
        subOctave = subOctaveIn;
    }
    
    void setAmpADSRParamPointers(std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* sustain, std::atomic<float>* release)
    {
        ampAttack = attack;
        ampDecay = decay;
        ampSustain = sustain;
        ampRelease = release;
    }
    
    void setDistParamPointers(std::atomic<float>* foldDistIn)
    {
        foldbackDistortion = foldDistIn;
    }
    
    // Modifiers
    void setRingModParamPointers(std::atomic<float>* ringPitch, std::atomic<float>* ringTone, std::atomic<float>* mix)
    {
        ringModPitch = ringPitch;
        ringModTone = ringTone;
        ringMix = mix;
    }
    
    void setFreqShiftParamPointers(std::atomic<float>* shiftPitch, std::atomic<float>* mix)
    {
        freqShiftPitch = shiftPitch;
        freqShiftMixVal = mix;
    }
    
    void setSampleAndHoldParamPointers(std::atomic<float>* pitch, std::atomic<float>* mix)
    {
        sAndHPitch = pitch;
        sAndHMixVal = mix;
    }
    
    // Filters
    void setFilterParamPointers(std::atomic<float>* cutoff, std::atomic<float>* res, std::atomic<float>* type)
    {
        filterCutoffFreq = cutoff;
        filterResonance = res;
        filterSelector = type;
    }
    
    void setFilterADSRParamPointers(std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* sustain, std::atomic<float>* release, std::atomic<float>* amtCO, std::atomic<float>* amtRes )
    {
        filterAttack = attack;
        filterDecay = decay;
        filterSustain = sustain;
        filterRelease = release;
        filterADSRCutOffAmount = amtCO;
        filterADSRResAmount = amtRes;
    }
    
    void setFilterLFOParamPointers(std::atomic<float>* freq, std::atomic<float>* amount, std::atomic<float>* shape)
    {
        filtLFOFreq = freq;
        filtLFOAmt = amount;
        filtLFOShape = shape;
    }
    
    void setPortamentoParamPointers(std::atomic<float>* portaTime)
    {
        portamentoAmount = portaTime;
    }
    
    void setMasterGainParamPointers(std::atomic<float>* gainAmt)
    {
        masterGainControl = gainAmt;
    }
    
    //
    // ADSR Values
    //
    
    // Main Osc ADSR
    void setAmpADSRValues()
    {
        //ADSR::Parameters envParams;
        envParams.attack = *ampAttack;      // time (sec)
        envParams.decay = *ampDecay;        // time (sec)
        envParams.sustain = *ampSustain;    // amplitude 0.0f to 1.0f
        envParams.release = *ampRelease;    // time (sec)
        
        env.setParameters(envParams);
    }
    
    void setFilterADSRValues()
    {
        ADSR::Parameters filtEnvParams;
        filtEnvParams.attack = *filterAttack;
        filtEnvParams.decay = *filterDecay;
        filtEnvParams.sustain = *filterSustain;
        filtEnvParams.release = *filterRelease;
        
        filtEnv.setParameters(filtEnvParams);
    }
    
    void setFiltLFOClickValues()
    {
        ADSR::Parameters filtLFOClickParams;
        filtLFOClickParams.attack = 0.02f;
        filtLFOClickParams.decay = 0.5f;
        filtLFOClickParams.sustain = 1.0f;
        filtLFOClickParams.release = 0.02f;
        
        filtLFOClickingEnv.setParameters(filtLFOClickParams);
    }
    
    void setPortamentoTime(float SR, float portaTime)
    {
        portamento.reset(SR, portaTime);
    }
    
    
    
    
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        ending = false;
        
        setPortamentoTime(sampleRate, *portamentoAmount);
        
        // Sets Amp ADSR for each note
        setAmpADSRValues();
        setFilterADSRValues();
        
        // Set Sub Octave
        incrementDenominator = subOscParamControl.subOctaveSelector(subOctave);
        
        // Converts incoming MIDI note to frequency
        freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        
        portamento.setTargetValue(freq);
        
        // Envelopes
        // Amp envelope
        env.noteOn();   // Start envelope
        
        //Filter Envelope
        filtEnv.noteOn();
        filtLFOClickingEnv.noteOn();
    
    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
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
    /**
     The Main DSP Block: Put My DSP code in here
     
     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            // Main Oscillator Wavetable Morph Values
            float sineLevel = oscParamControl.sinMorphGain(oscillatorMorph);
            float spikeLevel = oscParamControl.spikeMorphGain(oscillatorMorph);
            float sawLevel = oscParamControl.sawMorphGain(oscillatorMorph);
            
            // Sub Oscillator Wavetable Morph Values
            float sinSubLevel = subOscParamControl.sinSubGain(subOscMorph);
            float squareSubLevel = subOscParamControl.squareSubGain(subOscMorph);
            float sawSubLevel = subOscParamControl.sawSubGain(subOscMorph);
            
            // Ring Mod
            ringMod.setRingToneSlider(ringModTone);
            
            // Frequency Shifter
            freqShift.oscMorph(oscillatorMorph);        // oscillatorMorph same as main oscillator wavetables
            freqShift.modFreq(freq, freqShiftPitch);
            
            
            // Filter LFO
            filterLFO.setIncrement(*filtLFOFreq, 1.0f);
            float filtLFOSinLevel = filtLFOShapeControl.sinSubGain(filtLFOShape);
            float filtLFOSquareLevel = filtLFOShapeControl.squareSubGain(filtLFOShape);
            float filtLFOSawLevel = filtLFOShapeControl.sawSubGain(filtLFOShape);
            
            // Value smoothing
            foldbackDistortionSmooth.setTargetValue(*foldbackDistortion);
            subGainSmooth.setTargetValue(*subGain);
            ringMixSmooth.setTargetValue(*ringMix);
            freqShiftMixValSmooth.setTargetValue(*freqShiftMixVal);
            sAndHMixValSmooth.setTargetValue(*sAndHMixVal);
            masterGainControlSmooth.setTargetValue(*masterGainControl);
            
            
            // DSP!
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                float portaFreq = portamento.getNextValue();
                
                // Main Oscillators
                wtSine.setIncrement(portaFreq);
                wtSaw.setIncrement(portaFreq);
                wtSpike.setIncrement(portaFreq);
                subOsc.setIncrement(portaFreq, incrementDenominator);
                
                // Mod Oscs
                ringMod.modFreq(portaFreq, ringModPitch);
                freqShift.modFreq(portaFreq, freqShiftPitch);
                sAndH.modFreq(portaFreq, sAndHPitch);
                
                // Gets value of next sample in envelope. Use to scale volume
                float envVal = env.getNextSample();
                float filtEnvVal = filtEnv.getNextSample();
                float filtLFOEnvVal = filtLFOClickingEnv.getNextSample();
                
                //
                // Main Oscillator Wavetable Processing + Foldback Distortion
                //
                
                // osc wavetable values scaled by oscillatorMorph parameter
                float sinOscSample = wtSine.process() * sineLevel;
                float spikeOscSample = wtSpike.process() * spikeLevel;
                float sawOscSample = wtSaw.process() * sawLevel;
                
                // Combine wavetables and scale by half (only two play at once) scaled by envelope
                float oscSample = (sinOscSample + spikeOscSample + sawOscSample) * 0.5f;
                
                // Apply foldback distortion to main oscillator
                float foldbackDistortionSmoothed = foldbackDistortionSmooth.getNextValue();
                float oscDistSample = std::sin(oscSample * foldbackDistortionSmoothed);
        
                
                //
                // Modifier Processing: Currently Series. Make Parallel option?
                //
                
                // Ring Modulation
                float ringModSample = oscDistSample * ringMod.process();
                float ringMixValSmoothed = ringMixSmooth.getNextValue();
                float oscRingSample = ringModMix.dryWetMix(oscDistSample, ringModSample, ringMixValSmoothed);
                
                // Frequency Shifter
                float freqShiftSample = freqShift.process();
                float freqShiftMixValSmoothed = freqShiftMixValSmooth.getNextValue();
                float oscShiftSample = freqShiftMix.dryWetMix(oscRingSample, freqShiftSample, freqShiftMixValSmoothed);
                
                float sAndHSample = sAndH.processSH(oscShiftSample);
                float sAndHMixValSmoothed = sAndHMixValSmooth.getNextValue();
                float oscSandHSample = sAndHMix.dryWetMix(oscShiftSample, sAndHSample, sAndHMixValSmoothed);
                
                //
                // Sub Osc
                //
                
                // sub wavetable values morphed by subOscillatorMorph, scaled by subGain
                float subSample = subOsc.process(sinSubLevel, squareSubLevel, sawSubLevel) * subGainSmooth.getNextValue();//*subGain;
                
                //
                // (Main Osc + Foldback + Modifiers) + Sub Osc
                //
                
                // Combine main osc and sub values, scaled and enveloped
                float currentSample = (oscSandHSample + subSample) * 0.5f * envVal;
                
                //
                // Filters
                //
                
                // Filter LFO
                float filtLFOSample = filterLFO.process(filtLFOSinLevel, filtLFOSquareLevel, filtLFOSawLevel) * filtLFOEnvVal;
                
                // Selects filter type
                if ((int)*filterSelector == 0)
                {
                    filterSample = twoPoleLPF.processFilter(freq, filterCutoffFreq, filterResonance, currentSample, filtEnvVal, filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
                }
                else if ((int)*filterSelector == 1)
                {
                    filterSample = fourPoleLPF.processFilter(freq, filterCutoffFreq, filterResonance, currentSample, filtEnvVal, filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
                }
                else if ((int)*filterSelector == 2)
                {
                    filterSample = eightPoleLPF.processFilter(freq, filterCutoffFreq, filterResonance, currentSample, filtEnvVal, filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
                }
                else if ((int)*filterSelector == 3)
                {
                    filterSample = notchFilter.processFilter(freq, filterCutoffFreq, filterResonance, currentSample, filtEnvVal, filterADSRCutOffAmount, filterADSRResAmount, filtLFOSample, filtLFOAmt);
                }
                
                float masterSample = filterSample * masterGainControlSmooth.getNextValue();
                
                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan<outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, masterSample * 0.5f);
                }
                
                
                if (ending)
                {
                    if (envVal < 0.0001f)
                    {
                        // reset envelopes and flip playing bool
                        env.reset();
                        filtEnv.reset();
                        filtLFOClickingEnv.reset();
                        playing = false;
                    }
                }
                
                
            }
        }
    }
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of MySynthSound
     */
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<MySynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false;
    
    float freq;
    
    /// ADSR envelope instance
    ADSR env;
    ADSR filtEnv;
    ADSR filtLFOClickingEnv;
    
    // Wavetable Class Instance
    Wavetable wtSine;
    SawWavetable wtSaw;
    SpikeWavetable wtSpike;
    SubOsc subOsc;

    // Oscillator parameter members
    std::atomic<float>* oscillatorMorph;
    std::atomic<float>* subOscMorph;
    std::atomic<float>* subGain;
    SmoothedValue<float> subGainSmooth;
    std::atomic<float>* subOctave;
    std::atomic<float>* foldbackDistortion;
    int incrementDenominator;
    
    // Oscillator Parameter Controls
    OscParamControl oscParamControl;
    SubOscParamControl subOscParamControl;
    
    // Amp Envelope Parameter Controls
    std::atomic<float>* ampAttack;
    std::atomic<float>* ampDecay;
    std::atomic<float>* ampSustain;
    std::atomic<float>* ampRelease;
    ADSR::Parameters envParams;
    
    // Portamento
    SmoothedValue<float> portamento;
    std::atomic<float>* portamentoAmount;
    
    // Foldback Distortion
    SmoothedValue<float> foldbackDistortionSmooth;
    
    // Ring Mod Instances
    RingMod ringMod;
    DryWet ringModMix;
    
    // Ring Mod Parameters
    std::atomic<float>* ringModPitch;
    std::atomic<float>* ringModTone;
    std::atomic<float>* ringMix;
    SmoothedValue<float> ringMixSmooth;
    
    // Frequency Shifter Instances
    FrequencyShifter freqShift;
    DryWet freqShiftMix;
    
    // Frequency Shifter Parameters
    std::atomic<float>* freqShiftPitch;
    std::atomic<float>* freqShiftMixVal;
    SmoothedValue<float> freqShiftMixValSmooth;
    
    // Sample and Hold Instances
    SampleAndHold sAndH;
    DryWet sAndHMix;
    
    // Sample and Hold Parameters
    std::atomic<float>* sAndHPitch;
    std::atomic<float>* sAndHMixVal;
    SmoothedValue<float> sAndHMixValSmooth;
    
    
    // Filter Instances
    TwoPoleLPF twoPoleLPF;
    FourPoleLPF fourPoleLPF;
    EightPoleLPF eightPoleLPF;
    NotchFilter notchFilter;
    
    // Filter Parameters
    std::atomic<float>* filterCutoffFreq;
    std::atomic<float>* filterResonance;
    std::atomic<float>* filterSelector;
    float filterSample;
    
    // Filter Envelope Parameters
    std::atomic<float>* filterAttack;
    std::atomic<float>* filterDecay;
    std::atomic<float>* filterSustain;
    std::atomic<float>* filterRelease;
    std::atomic<float>* filterADSRCutOffAmount;
    std::atomic<float>* filterADSRResAmount;
    
    // Filter LFO Instance
    SubOsc filterLFO;
    
    // Filter LFO Parameters
    std::atomic<float>* filtLFOFreq;
    std::atomic<float>* filtLFOAmt;
    std::atomic<float>* filtLFOShape;
    SubOscParamControl filtLFOShapeControl;
    
    // Master Gain
    float masterGain;
    std::atomic<float>* masterGainControl;
    SmoothedValue<float> masterGainControlSmooth;
    
    
    
    // Master Sample Rate
    float sampleRate;

};
