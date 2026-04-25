/*
  ==============================================================================

    BassSynthVoice.h
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

class BassSynthSound
    : public juce::SynthesiserSound
{
public:
    bool appliesToNote    (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

//======================================================

class BassSynthVoice
    : public juce::SynthesiserVoice
{
public:
    BassSynthVoice();

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

    void startNote(int                       midiNoteNumber
                   , float                   velocity
                   , juce::SynthesiserSound*
                   , int                     currentPitchWheelPosition) override;

    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    void updatePitchBendRange(float newRange);

    void controllerMoved(int, int) override {}

    bool canPlaySound(juce::SynthesiserSound* sound) override;

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

    float PitchBendCents();
    float CalcShiftHz(float centsOffset);
    void  SetPitchBend(int pitchWheelPos);

    bool playing;
    bool ending;

    // Playback note
    float freq;
    float vel;                  // velocity 0-1
    float pitchBend;
    float shiftHz;
    float previousPitchWheelValue;
    float pitchBendSemitones;
    float pitchBendUpSemitones;
    float pitchBendDownSemitones;
    float lastRecievedPitchWheelValue;

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
