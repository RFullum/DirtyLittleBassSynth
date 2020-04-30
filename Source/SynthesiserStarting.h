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
    
    void init(float sampleRate)
    {
        //
        // Sets sampleRates here
        //
        
        // Main and Sub Osc
        env.setSampleRate(sampleRate);
        wtSine.setSampleRate(sampleRate);
        wtSaw.setSampleRate(sampleRate);
        wtSpike.setSampleRate(sampleRate);
        subOsc.setSampleRate(sampleRate);
        
        // Modifiers
        ringMod.setSampleRate(sampleRate);
        freqShift.setSampleRate(sampleRate);
        sAndH.setSampleRate(sampleRate);
        
        // populates wavetables
        wtSine.populateWavetable();
        wtSaw.populateWavetable();
        wtSpike.populateWavetable();
        subOsc.populateWavetable();
        
        
        ADSR::Parameters envParams;
        envParams.attack = 0.1f;    // time
        envParams.decay = 0.25f;    // time
        envParams.sustain = 0.5f;   // amplitude
        envParams.release = 0.25f;  // time
        
        env.setParameters(envParams);
    }
    
    void setOscParamPointers(std::atomic<float>* oscMorphIn, std::atomic<float>* subOscMorphIn, std::atomic<float>* subOscGainIn, std::atomic<float>* subOctaveIn)
    {
        oscillatorMorph = oscMorphIn;
        subOscMorph = subOscMorphIn;
        subGain = subOscGainIn;
        subOctave = subOctaveIn;
    }
    
    void setDistParamPointers(std::atomic<float>* foldDistIn)
    {
        foldbackDistortion = foldDistIn;
    }
    
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
    
    /*
    void setParameterPointers(std::atomic<float>* detuneIn)
    {
        detuneAmount = detuneIn;
    }
    */
    
    
    
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
        
        // Set Sub Octave
        int incrementDenominator = subOscParamControl.subOctaveSelector(subOctave);
        
        // Converts incoming MIDI note to frequency
        freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        
        
        // Main Oscillators
        wtSine.setIncrement(freq);
        wtSaw.setIncrement(freq);
        wtSpike.setIncrement(freq);
        subOsc.setIncrement(freq, incrementDenominator);
        
        
        env.reset();    // Resets note
        env.noteOn();   // Start envelope
    
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
            // allows params to adjust detune amount while playing
            // Doing it up here does it once a block. Inside the DSP! loop
            // does it every sample. 
            
            //detuneOsc.setFrequency(freq - *detuneAmount);
            
            // Main Oscillator Wavetable Morph Values
            float sineLevel = oscParamControl.sinMorphGain(oscillatorMorph);
            float spikeLevel = oscParamControl.spikeMorphGain(oscillatorMorph);
            float sawLevel = oscParamControl.sawMorphGain(oscillatorMorph);
            
            // Sub Oscillator Wavetable Morph Values
            float sinSubLevel = subOscParamControl.sinSubGain(subOscMorph);
            float squareSubLevel = subOscParamControl.squareSubGain(subOscMorph);
            float sawSubLevel = subOscParamControl.sawSubGain(subOscMorph);
            
            // Ring Mod
            ringMod.modFreq(freq, ringModPitch);
            ringMod.setRingToneSlider(ringModTone);
            
            // Frequency Shifter
            freqShift.oscMorph(oscillatorMorph);        // oscillatorMorph same as main oscillator wavetables
            freqShift.modFreq(freq, freqShiftPitch);
            
            // Sample and Hold
            sAndH.modFreq(freq, sAndHPitch);
            
            // DSP!
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                
                // Gets value of next sample in envelope. Use to scale volume
                float envVal = env.getNextSample();
                
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
                float oscDistSample = std::sin(oscSample * *foldbackDistortion);
                
                //
                // Modifier Processing: Currently Series. Make Parallel option?
                //
                
                // Ring Modulation
                float ringModSample = oscDistSample * ringMod.process();
                float oscRingSample = ringModMix.dryWetMix(oscDistSample, ringModSample, ringMix);
                
                // Frequency Shifter
                float freqShiftSample = freqShift.process();   //  + oscRingSample; ???
                float oscShiftSample = freqShiftMix.dryWetMix(oscRingSample, freqShiftSample, freqShiftMixVal);
                
                float sAndHSample = sAndH.processSH(oscShiftSample);
                float oscSandHSample = sAndHMix.dryWetMix(oscShiftSample, sAndHSample, sAndHMixVal);
                
                //
                // Sub Osc
                //
                
                // sub wavetable values morphed by subOscillatorMorph, scaled by subGain
                float subSample = subOsc.process(sinSubLevel, squareSubLevel, sawSubLevel) * *subGain;
                
                // Combine main osc and sub values, scaled and enveloped
                float currentSample = (oscSandHSample + subSample) * 0.5f * envVal; //( oscSample + subSample ) * 0.5f * envVal;
                
                
                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan<outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, currentSample * 0.2);
                }
                
                
                if (ending)
                {
                    if (envVal < 0.0001f)
                    {
                        // clearCurrentNote();
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
    
    // Wavetable Class Instance and dependencies
    Wavetable wtSine;
    SawWavetable wtSaw;
    SpikeWavetable wtSpike;
    SubOsc subOsc;

    // Oscillator parameter members
    std::atomic<float>* oscillatorMorph;
    std::atomic<float>* subOscMorph;
    std::atomic<float>* subGain;
    std::atomic<float>* subOctave;
    std::atomic<float>* foldbackDistortion;
    
    // Oscillator Parameter Controls
    OscParamControl oscParamControl;
    SubOscParamControl subOscParamControl;
    
    // Ring Mod Instances
    RingMod ringMod;
    DryWet ringModMix;
    
    // Ring Mod Parameters
    std::atomic<float>* ringModPitch;
    std::atomic<float>* ringModTone;
    std::atomic<float>* ringMix;
    
    // Frequency Shifter Instances
    FrequencyShifter freqShift;
    DryWet freqShiftMix;
    
    // Frequency Shifter Parameters
    std::atomic<float>* freqShiftPitch;
    std::atomic<float>* freqShiftMixVal;
    
    // Sample and Hold Instances
    SampleAndHold sAndH;
    DryWet sAndHMix;
    
    // Sample and Hold Parameters
    std::atomic<float>* sAndHPitch;
    std::atomic<float>* sAndHMixVal;
    

};
