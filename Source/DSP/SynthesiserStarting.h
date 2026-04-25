/*
  ==============================================================================

    MySynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd, modified by Robert Fullum

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

//======================================================

class MySynthSound
    : public juce::SynthesiserSound
{
public:
    bool appliesToNote    (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

//======================================================

/*!
 @class MySynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple MySynthVoice objects will be created by the juce::Synthesiser so that it can be played polyphicially
 
 @namespace none
 @updated 2019-06-18
 */
class MySynthVoice
    : public juce::SynthesiserVoice
{
public:
    MySynthVoice();
    
    void Init(float SR, int blockSize);
    
    void SetOscParamPointers(std::atomic<float>   *oscMorphIn
                             , std::atomic<float> *subOscMorphIn
                             , std::atomic<float> *subOscGainIn
                             , std::atomic<float> *subOctaveIn);
    void SetAmpADSRParamPointers(std::atomic<float>   *attack
                                 , std::atomic<float> *decay
                                 , std::atomic<float> *sustain
                                 , std::atomic<float> *release);
    void SetDistParamPointers(std::atomic<float> *foldDistIn);
    void SetRingModParamPointers(std::atomic<float>   *ringPitch
                                 , std::atomic<float> *ringTone
                                 , std::atomic<float> *mix);
    void SetFreqShiftParamPointers(std::atomic<float> *shiftPitch, std::atomic<float> *mix);
    void SetSampleAndHoldParamPointers(std::atomic<float> *pitch, std::atomic<float> *mix);
    void SetFilterParamPointers(std::atomic<float>   *cutoff
                                , std::atomic<float> *res
                                , std::atomic<float> *type);
    void SetFilterADSRParamPointers(std::atomic<float>   *attack
                                    , std::atomic<float> *decay
                                    , std::atomic<float> *sustain
                                    , std::atomic<float> *release
                                    , std::atomic<float> *amtCO
                                    , std::atomic<float> *amtRes);
    void SetFilterLFOParamPointers(std::atomic<float>   *freq
                                   , std::atomic<float> *amount
                                   , std::atomic<float> *shape);
    void SetPortamentoParamPointers(std::atomic<float> *portaTime);
    void SetMasterGainParamPointers(std::atomic<float> *gainAmt);
    void SetFilterSpec(float &sampRate, float &sampleSize);
    void SetAmpADSRValues();
    void SetFilterADSRValues();
    void SetFiltLFOClickValues();
    void SetPortamentoTime(float SR, float portaTime);
    
    
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param juce::SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote(int                       midiNoteNumber
                   , float                   velocity
                   , juce::SynthesiserSound*
                   , int                     currentPitchWheelPosition) override;
    
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
    void renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples) override;
    
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
    bool canPlaySound (juce::SynthesiserSound* sound) override;

private:
    // Per-block oscillator morph levels for main / sub / filter-LFO osc banks.
    struct BlockLevels
    {
        float mainSin, mainSpike, mainSaw;
        float subSin,  subSquare, subSaw;
        float lfoSin,  lfoSquare, lfoSaw;
    };

    // Block-stage helpers (called once per block from renderNextBlock).
    BlockLevels ComputeBlockLevels();
    void        PrepareDspForBlock(const BlockLevels &levels);

    // Per-sample-stage helpers (called once per sample from the render loop).
    float ProcessMainOscSample(float envVal, const BlockLevels &levels);
    float ProcessModifierChain(float input, float envVal);
    float ProcessSubOscSample(float envVal, const BlockLevels &levels);
    float ProcessFilterChain(float input, float filtEnvVal, float filtLFOEnvVal, const BlockLevels &levels);

    // Block-cached parameter values (loaded once per block in PrepareDspForBlock so
    // the per-sample loop never dereferences std::atomic<float>* on the audio thread).
    float ringModPitchVal           = 0.0f;
    float freqShiftPitchVal         = 0.0f;
    float sAndHPitchVal             = 0.0f;
    int   filterSelectorIndex       = 0;
    float filterResonanceVal        = 0.0f;
    float filterADSRCutOffAmountVal = 0.0f;
    float filterADSRResAmountVal    = 0.0f;
    float filtLFOAmtVal             = 0.0f;

    // Selected filter for the current block. Set in PrepareDspForBlock so the per-sample
    // loop calls ProcessFilter via virtual dispatch instead of switching on every sample.
    Filter *activeFilter = nullptr;

    // Private Pitch Bend methods
    /// maps pitchwheel min/max positions to bend in cents as a function of pitchBend
    float PitchBendCents();
    
    /// calculates pitch wheel's shift in hz
    float CalcShiftHz(float centsOffset);
    
    /// Pitch wheel position to pitchBend up or down
    void SetPitchBend(int pitchWheelPos);
    
    //--------------------------------------------------------------------------
    // Are the voices playing:
    bool playing;
    bool ending;
    
    // Playback note
    float freq;
    float vel;                        // velocity 0-1
    float pitchBend;
    float shiftHz;
    float previousPitchWheelValue;
    float pitchBendSemitones;
    float pitchBendUpSemitones;
    float pitchBendDownSemitones;
    float lastRecievedPitchWheelValue;
    
    // Host Transport Data
    //float hostBPM = 120.0f;
    
    /// juce::ADSR envelope instances
    juce::ADSR env;
    juce::ADSR filtEnv;
    juce::ADSR filtLFOClickingEnv;
    
    // Wavetable Class Instances
    Wavetable wtSine;
    SawWavetable wtSaw;
    SpikeWavetable wtSpike;
    SubOsc subOsc;

    // Oscillator parameter members
    std::atomic<float>         *oscillatorMorph;
    std::atomic<float>         *subOscMorph;
    std::atomic<float>         *subGain;
    juce::SmoothedValue<float>  subGainSmooth;
    std::atomic<float>         *subOctave;
    std::atomic<float>         *foldbackDistortion;
    int                         incrementDenominator;
    
    // Oscillator Parameter Controls
    OscParamControl oscParamControl;
    SubOscParamControl subOscParamControl;
    
    // Amp Envelope Parameter Controls
    std::atomic<float>     *ampAttack;
    std::atomic<float>     *ampDecay;
    std::atomic<float>     *ampSustain;
    std::atomic<float>     *ampRelease;
    juce::ADSR::Parameters  envParams;
    
    // Portamento
    juce::SmoothedValue<float> portamento;
    std::atomic<float> *portamentoAmount;
    
    // Foldback Distortion
    juce::SmoothedValue<float> foldbackDistortionSmooth;
    
    // Ring Mod Instances
    RingMod ringMod;

    // Ring Mod Parameters
    std::atomic<float>         *ringModPitch;
    std::atomic<float>         *ringModTone;
    std::atomic<float>         *ringMix;
    juce::SmoothedValue<float>  ringMixSmooth;
    
    // Frequency Shifter Instances
    FrequencyShifter freqShift;

    // Frequency Shifter Parameters
    std::atomic<float>         *freqShiftPitch;
    std::atomic<float>         *freqShiftMixVal;
    juce::SmoothedValue<float>  freqShiftMixValSmooth;
    
    // Sample and Hold Instances
    SampleAndHold sAndH;

    // Sample and Hold Parameters
    std::atomic<float>         *sAndHPitch;
    std::atomic<float>         *sAndHMixVal;
    juce::SmoothedValue<float>  sAndHMixValSmooth;
    
    
    // Filter Instances
    TwoPoleLPF twoPoleLPF;
    FourPoleLPF fourPoleLPF;
    EightPoleLPF eightPoleLPF;
    NotchFilter notchFilter;
    
    
    // Filter Parameters
    std::atomic<float>         *filterCutoffFreq;
    juce::SmoothedValue<float>  filterCutoffFreqSmooth;
    std::atomic<float>         *filterResonance;
    std::atomic<float>         *filterSelector;
    
    // Filter Envelope Parameters
    std::atomic<float> *filterAttack;
    std::atomic<float> *filterDecay;
    std::atomic<float> *filterSustain;
    std::atomic<float> *filterRelease;
    std::atomic<float> *filterADSRCutOffAmount;
    std::atomic<float> *filterADSRResAmount;
    
    // Filter LFO Instance
    SubOsc filterLFO;
    
    // Filter LFO Parameters
    std::atomic<float> *filtLFOFreq;
    std::atomic<float> *filtLFOAmt;
    std::atomic<float> *filtLFOShape;
    SubOscParamControl  filtLFOShapeControl;
    
    // Master Gain
    float masterGain;
    std::atomic<float>         *masterGainControl;
    juce::SmoothedValue<float>  masterGainControlSmooth;
    juce::SmoothedValue<float>  velocitySmooth;
    
    // Master Sample Rate
    float sampleRate;
    int samplesPerBlock;
};
