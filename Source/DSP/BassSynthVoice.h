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
#include "TempoSnapshot.h"

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

    void startNote(int                       midiNoteNumber
                   , float                   velocity
                   , juce::SynthesiserSound*
                   , int                     currentPitchWheelPosition) override;

    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int, int) override {}

    bool canPlaySound(juce::SynthesiserSound* sound) override;

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
    
    void SetFilterLFOSyncParamPointers(std::atomic<float> *syncOn, std::atomic<float> *syncDivIndex);
    void SetTempoSnapshot(const TempoSnapshot *snapshot);
    void SetPortamentoParamPointers(std::atomic<float> *portaTime);
    void SetPortamentoModeParamPointers(std::atomic<float> *portaOn, std::atomic<float> *portaLegato);
    void SetMasterGainParamPointers(std::atomic<float> *gainAmt);
    void SetFilterSpec(float &sampRate, float &sampleSize);
    void SetAmpADSRValues();
    void SetFilterADSRValues();
    void SetFiltLFOClickValues();
    void SetPortamentoTime(float SR, float portaTime);
    void updatePitchBendRange(float newRange);

    /// Latency introduced by the oversampling stage, in *base-rate* samples.
    /// Reported to the host so it can compensate. Returns 0 before Init has run.
    int GetOversamplingLatencyInSamples() const noexcept;

private:
    // Per-block oscillator morph levels for main / sub / filter-LFO osc banks.
    struct BlockLevels
    {
        float mainSin, mainSpike, mainSaw;
        float subSin,  subSquare, subSaw;
        float lfoSin,  lfoSquare, lfoSaw;
    };
    
    // Foldback distortion + the modifier chain are run at 4× base sample rate to
    // suppress the alias distortion their non-linearities would otherwise create.
    // Wavetable / sub osc / filter / master gain stay at base rate.
    static constexpr int oversamplingFactorLog2 = 2;                              // 2 → 4×
    static constexpr int oversamplingFactor     = 1 << oversamplingFactorLog2;    // 4
    
    BlockLevels ComputeBlockLevels();
    void        PrepareDspForBlock(const BlockLevels &levels);
    
    float ProcessSubOscSample(float envVal, const BlockLevels &levels);
    float ProcessFilterChain(float input, float filtEnvVal, float filtLFOEnvVal, const BlockLevels &levels);

    float PitchBendCents();
    float CalcShiftHz(float centsOffset);
    void  SetPitchBend(int pitchWheelPos);
    
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    juce::AudioBuffer<float>                        preFoldbackBuf;

    // Per-base-sample state cached during pass 1 so pass 2 (running at OSR)
    // doesn't try to advance these once per oversampled sample.
    std::vector<float> envValsCache;
    std::vector<float> filtEnvValsCache;
    std::vector<float> filtLFOEnvValsCache;
    std::vector<float> foldbackCache;
    std::vector<float> ringMixCache;
    std::vector<float> freqShiftMixCache;
    std::vector<float> sAndHMixCache;

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

    bool playing = false;
    bool ending  = false;

    // Playback note
    float freq                        = 0.0f;
    float vel                         = 0.0f;   // velocity 0-1
    float pitchBend                   = 0.0f;
    float shiftHz                     = 1.0f;
    float previousPitchWheelValue     = 0.0f;
    float pitchBendSemitones          = 12.0f;
    float pitchBendUpSemitones        = 12.0f;
    float pitchBendDownSemitones      = 12.0f;
    float lastReceivedPitchWheelValue = 0.0f;

    juce::ADSR env;
    juce::ADSR filtEnv;
    juce::ADSR filtLFOClickingEnv;
    
    // Wavetable Class Instances
    Wavetable wtSine;
    SawWavetable wtSaw;
    SpikeWavetable wtSpike;
    SubOsc subOsc;

    // Oscillator parameter members
    std::atomic<float>         *oscillatorMorph      = nullptr;
    std::atomic<float>         *subOscMorph          = nullptr;
    std::atomic<float>         *subGain              = nullptr;
    juce::SmoothedValue<float>  subGainSmooth;
    std::atomic<float>         *subOctave            = nullptr;
    std::atomic<float>         *foldbackDistortion   = nullptr;
    int                         incrementDenominator = 0;
    
    // Oscillator Parameter Controls
    OscParamControl oscParamControl;
    SubOscParamControl subOscParamControl;
    
    // Amp Envelope Parameter Controls
    std::atomic<float>     *ampAttack  = nullptr;
    std::atomic<float>     *ampDecay   = nullptr;
    std::atomic<float>     *ampSustain = nullptr;
    std::atomic<float>     *ampRelease = nullptr;
    juce::ADSR::Parameters  envParams;

    // Portamento
    juce::SmoothedValue<float> portamento;
    std::atomic<float> *portamentoAmount = nullptr;
    std::atomic<float> *portamentoOnParam     = nullptr;
    std::atomic<float> *portamentoLegatoParam = nullptr;
    bool                portaEverPlayed       = false;
    bool                portaNoteWasReleased  = true;
    
    // Foldback Distortion
    juce::SmoothedValue<float> foldbackDistortionSmooth;
    
    // Ring Mod Instances
    RingMod ringMod;

    // Ring Mod Parameters
    std::atomic<float>         *ringModPitch = nullptr;
    std::atomic<float>         *ringModTone  = nullptr;
    std::atomic<float>         *ringMix      = nullptr;
    juce::SmoothedValue<float>  ringMixSmooth;

    // Frequency Shifter Instances
    FrequencyShifter freqShift;

    // Frequency Shifter Parameters
    std::atomic<float>         *freqShiftPitch  = nullptr;
    std::atomic<float>         *freqShiftMixVal = nullptr;
    juce::SmoothedValue<float>  freqShiftMixValSmooth;

    // Sample and Hold Instances
    SampleAndHold sAndH;

    // Sample and Hold Parameters
    std::atomic<float>         *sAndHPitch  = nullptr;
    std::atomic<float>         *sAndHMixVal = nullptr;
    juce::SmoothedValue<float>  sAndHMixValSmooth;
    
    
    // Filter Instances
    TwoPoleLPF twoPoleLPF;
    FourPoleLPF fourPoleLPF;
    EightPoleLPF eightPoleLPF;
    NotchFilter notchFilter;
    
    
    // Filter Parameters
    std::atomic<float>         *filterCutoffFreq = nullptr;
    juce::SmoothedValue<float>  filterCutoffFreqSmooth;
    std::atomic<float>         *filterResonance  = nullptr;
    std::atomic<float>         *filterSelector   = nullptr;

    // Filter Envelope Parameters
    std::atomic<float> *filterAttack           = nullptr;
    std::atomic<float> *filterDecay            = nullptr;
    std::atomic<float> *filterSustain          = nullptr;
    std::atomic<float> *filterRelease          = nullptr;
    std::atomic<float> *filterADSRCutOffAmount = nullptr;
    std::atomic<float> *filterADSRResAmount    = nullptr;

    // Filter LFO Instance
    SubOsc filterLFO;

    // Filter LFO Parameters
    std::atomic<float> *filtLFOFreq  = nullptr;
    std::atomic<float> *filtLFOAmt   = nullptr;
    std::atomic<float> *filtLFOShape = nullptr;
    SubOscParamControl  filtLFOShapeControl;

    // Sync mode (filtLFO_sync, filtLFO_sync_div) + host transport snapshot.
    std::atomic<float>  *filtLFOSyncOn       = nullptr;
    std::atomic<float>  *filtLFOSyncDivIndex = nullptr;
    const TempoSnapshot *tempoSnapshotPtr    = nullptr;
    
    // Master Gain
    float                       masterGain        = 0.0f;
    std::atomic<float>         *masterGainControl = nullptr;
    juce::SmoothedValue<float>  masterGainControlSmooth;
    juce::SmoothedValue<float>  velocitySmooth;

    // Master Sample Rate
    float sampleRate      = 44100.0f;
    int   samplesPerBlock = 0;
};
