/*
  ==============================================================================

    FilterSection.h
    Created: 30 Apr 2020 6:33:55pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

/// Shared cutoff/resonance/LFO modulation state used by the IIR-based filters.
struct FilterModulation
{
    float sampleRate    = 44100.0f;
    float maxCutoff     = 18000.0f;
    float minCutoff     = 20.0f;
    float maxResonance  = 3.0f;

    // User-set base values
    float cutoffFreq = 1000.0f;
    float resonance  = 1.0f;

    // Output of ApplyEnvAndLfo (drives IIR coefficients)
    float cutoffLFO      = 1000.0f;
    float resonanceScale = 0.1f;

    // Coefficient-update change-detection
    float cutoffLFOPrev      = 1000.0f;
    float resonanceScalePrev = 0.1f;

    /// Maps normalised cutoff position (0..1) to a frequency, key-tracked. Floor is
    /// two octaves below the played note; ceiling is maxCutoff. Mapping is exponential
    /// so each equal-width slice of the slider covers the same number of octaves.
    void KeyMapTracked(float noteFreq, float cutoffPos);

    /// Maps normalised cutoff position (0..1) to a frequency, not key-tracked.
    /// Floor is minCutoff (20 Hz), ceiling is maxCutoff. Exponential mapping.
    void KeyMapFixed(float cutoffPos);

    /// Combines envelope and LFO modulation onto cutoffLFO and resonanceScale.
    void ApplyEnvAndLfo(float envVal, float amtToCO, float amtToRes, float lfoVal, float amtToLFO);

    /// True if cutoffLFO or resonanceScale changed since the last AcknowledgeUpdate.
    bool CoefficientsNeedUpdate() const;

    /// Stores current cutoffLFO / resonanceScale as the new "previous" baseline.
    void AcknowledgeUpdate();
};

//==============================================================================

/// Abstract filter interface used by BassSynthVoice's per-block activeFilter dispatch.
class Filter
{
public:
    virtual ~Filter() = default;

    virtual void  SetSampleRate(float SR) = 0;
    virtual float ProcessFilter(float   noteFreq
                                , float cutoff
                                , float res
                                , float sampleIn
                                , float envVal
                                , float amtToCO
                                , float amtToRes
                                , float lfoVal
                                , float amtToLFO) = 0;
};

//==============================================================================

class TwoPoleLPF
    : public Filter
{
public:
    TwoPoleLPF();

    void  SetSampleRate(float SR) override;
    float ProcessFilter(float   noteFreq
                        , float cutoff
                        , float res
                        , float sampleIn
                        , float envVal
                        , float amtToCO
                        , float amtToRes
                        , float lfoVal
                        , float amtToLFO) override;

private:
    FilterModulation mod;
    juce::IIRFilter  lowPass;
};

//==============================================================================

class FourPoleLPF
    : public Filter
{
public:
    void  SetSampleRate(float SR) override;
    float ProcessFilter(float   noteFreq
                        , float cutoff
                        , float res
                        , float sampleIn
                        , float envVal
                        , float amtToCO
                        , float amtToRes
                        , float lfoVal
                        , float amtToLFO) override;

private:
    TwoPoleLPF stage1;
    TwoPoleLPF stage2;
};

//==============================================================================

class EightPoleLPF
    : public Filter
{
public:
    void  SetSampleRate(float SR) override;
    float ProcessFilter(float   noteFreq
                        , float cutoff
                        , float res
                        , float sampleIn
                        , float envVal
                        , float amtToCO
                        , float amtToRes
                        , float lfoVal
                        , float amtToLFO) override;

private:
    FourPoleLPF stage1;
    FourPoleLPF stage2;
};

//==============================================================================

class NotchFilter
    : public Filter
{
public:
    void  SetSampleRate(float SR) override;
    float ProcessFilter(float   noteFreq
                        , float cutoff
                        , float res
                        , float sampleIn
                        , float envVal
                        , float amtToCO
                        , float amtToRes
                        , float lfoVal
                        , float amtToLFO) override;

private:
    FilterModulation mod;
    juce::IIRFilter  notchFilter;
};
