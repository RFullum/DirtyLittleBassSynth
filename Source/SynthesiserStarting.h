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
    MySynthVoice();
    
    void init(float SR, int blockSize);
    
    //
    // Parameter Pointers Setup
    //
    
    // Main Oscs
    void setOscParamPointers(std::atomic<float>* oscMorphIn, std::atomic<float>* subOscMorphIn,
                             std::atomic<float>* subOscGainIn, std::atomic<float>* subOctaveIn);
    
    void setAmpADSRParamPointers(std::atomic<float>* attack, std::atomic<float>* decay,
                                 std::atomic<float>* sustain, std::atomic<float>* release);
    
    void setDistParamPointers(std::atomic<float>* foldDistIn);
    
    // Modifiers
    void setRingModParamPointers(std::atomic<float>* ringPitch, std::atomic<float>* ringTone, std::atomic<float>* mix);
    
    void setFreqShiftParamPointers(std::atomic<float>* shiftPitch, std::atomic<float>* mix);
    
    void setSampleAndHoldParamPointers(std::atomic<float>* pitch, std::atomic<float>* mix);
    
    // Filters
    void setFilterParamPointers(std::atomic<float>* cutoff, std::atomic<float>* res, std::atomic<float>* type);
    
    void setFilterADSRParamPointers(std::atomic<float>* attack, std::atomic<float>* decay,
                                    std::atomic<float>* sustain, std::atomic<float>* release,
                                    std::atomic<float>* amtCO, std::atomic<float>* amtRes );
    
    void setFilterLFOParamPointers(std::atomic<float>* freq, std::atomic<float>* amount, std::atomic<float>* shape);
    
    void setPortamentoParamPointers(std::atomic<float>* portaTime);
    
    void setMasterGainParamPointers(std::atomic<float>* gainAmt);
    
    void setFilterSpec(float& sampRate, float& sampleSize);
    
    void setPlayheadInfo(AudioPlayHead::CurrentPositionInfo& playhead);
    
    
    
    
    //
    // ADSR Values
    //
    
    // Main Osc ADSR
    void setAmpADSRValues();
    
    /// Sets ADSR values for filter
    void setFilterADSRValues();
    
    /// Applies ADSR to LFO to avoid clicking
    void setFiltLFOClickValues();
    
    /// Sets up the portamentoTime 
    void setPortamentoTime(float SR, float portaTime);
    
    
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int currentPitchWheelPosition) override;
    
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override;
    
    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put My DSP code in here
     
     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    
    //--------------------------------------------------------------------------
    
    // Public Pitch Bend Methods
    /// synth class automatically sends newPitchWheelValue from its render block
    void pitchWheelMoved(int newPitchWheelValue) override; //{}
    
    /// Updates the number of semitones the pitchWheel will bend
    void updatePitchBendRange(float newRange);
    
    
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of MySynthSound
     */
    bool canPlaySound (SynthesiserSound* sound) override;
    
    //--------------------------------------------------------------------------
    
    AudioBuffer<float> oscVisualBuffer();
    AudioBuffer<float> subVisualBuffer();
    AudioBuffer<float> lfoVisualBuffer();
    

private:
    /// Populates the waveshape buffers
    void populateShape(AudioBuffer<float>& buf, float& sin, float& spikeSqr, float& saw, bool isSubOsc);
    
    // Private Pitch Bend methods
    /// maps pitchwheel min/max positions to bend in cents as a function of pitchBend
    float pitchBendCents();
    
    /// calculates pitch wheel's shift in hz
    float calcShiftHz(float centsOffset);
    
    /// Pitch wheel position to pitchBend up or down
    void setPitchBend(int pitchWheelPos);
    
    //--------------------------------------------------------------------------
    // Are the voices playing:
    bool playing;
    bool ending;
    
    // Playback note
    float freq;
    float vel;                                  // velocity 0-1
    float pitchBend;
    float shiftHz                     = 1.0f;
    float previousPitchWheelValue     = 0.0f;
    float pitchBendSemitones          = 12.0f;
    float pitchBendUpSemitones        = 12.0f;
    float pitchBendDownSemitones      = 12.0f;
    float lastRecievedPitchWheelValue = 0.0f;
    
    // Host Transport Data
    float hostBPM = 120.0f;
    
    /// ADSR envelope instances
    ADSR env;
    ADSR filtEnv;
    ADSR filtLFOClickingEnv;
    
    // Wavetable Class Instances
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
    SmoothedValue<float> filterCutoffFreqSmooth;
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
    int samplesPerBlock;
    
    // Waveshape Drawing
    AudioBuffer<float> mainOscShape;
    AudioBuffer<float> subOscShape;
    AudioBuffer<float> lfoOscShape;

};
