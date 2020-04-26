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
        // Sets sampleRates here
        sinOsc.setSampleRate(sampleRate);
        env.setSampleRate(sampleRate);
        wtSynth.setSampleRate(sampleRate);
        
        // populates wavetables
        wtSynth.populateWavetables();
        
        
        ADSR::Parameters envParams;
        envParams.attack = 0.1f;    // time
        envParams.decay = 0.25f;    // time
        envParams.sustain = 0.5f;   // amplitude
        envParams.release = 0.25f;  // time
        
        env.setParameters(envParams);
        
        
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
        
        // Converts incoming MIDI note to frequency
        float freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        sinOsc.setFrequency(freq);
        wtSynth.setIncrement(freq);
        
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
        
        
        // We'll come back to these later.
        // clearCurrentNote();
        // playing = false;
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
            
            
            // DSP!
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                
                // Gets value of next sample in envelope. Use to scale volume
                float envVal = env.getNextSample();
                
                // oscillator values scaled by number of oscs and envelope value
                //float currentSample = sinOsc.process() * envVal;
                float currentSample = wtSynth.process() * envVal;
                
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
    Wavetable wtSynth;
    
    SinOsc sinOsc;
    

};