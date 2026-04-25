/*
  ==============================================================================

    Modifiers.cpp
    Created: 26 Dec 2020 12:19:57pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Modifiers.h"



//==============================================================================
//==============================================================================


//
// Ring Mod
//

/// Constructor
RingMod::RingMod() : sampleRate(44100.0f), modFrequency(0.0f), ringToneSlider(0.0f) {}

// Destructo!
RingMod::~RingMod(){}

/// Sets sampleRates and set up wavetables
void RingMod::setSampleRate(float SR)
{
    sampleRate = SR;
    setUpWavetables();
}

/// Takes the note frequency to set wavetable increment
void RingMod::modFreq(float fqncy, std::atomic<float>* offset)
{
    modFrequency = fqncy * *offset;
    setRingIncrement();
}

// Setter for ringToneSlider value
void RingMod::setRingToneSlider(std::atomic<float>* toneSlider)
{
    ringToneSlider = *toneSlider;
}

/// Process wavetable: toneSlider controls amount of Sin/Square wave
float RingMod::process()
{
    return ringModProcess();
}

/// sets sampleRate for member wavetables and calls populate function
void RingMod::setUpWavetables()
{
    // Sets wavetable samplerate
    wtSine.SetSampleRate   (sampleRate);
    wtSquare.SetSampleRate (sampleRate);
    
    // Populates wavetables
    wtSine.PopulateWavetable();
    wtSquare.PopulateWavetable();
}

/// Sets the playback frequency for wavetables
void RingMod::setRingIncrement()
{
    wtSine.SetIncrement   (modFrequency);
    wtSquare.SetIncrement (modFrequency);
}

/// Calculate ring modulation: calls wavetable process functions, morphs their shapes, and returns sample value
float RingMod::ringModProcess()
{
    float sinVal = wtSine.Process();
    float sqVal  = wtSquare.Process();
    float outVal = toneControl.dryWetMix(sinVal, sqVal, ringToneSlider);
    
    return outVal;
}


//==============================================================================
//==============================================================================


//
// Frequency Shifter
//

/// Constructor
FrequencyShifter::FrequencyShifter() : sineLevel(1.0f), spikeLevel(0.0f), sawLevel(0.0f) {}

/// Sets sampleRate for frequency shifter wavetables and populates wavetables
void FrequencyShifter::setSampleRate(float SR)
{
    sampleRate = SR;
    setUpFreqShiftWavetables();
}

/// Takes the note frequency to set wavetable increment
void FrequencyShifter::modFreq(float fqncy, std::atomic<float>* offset)
{
    modFrequency = fqncy * *offset;
    setFreqShiftIncrement();
}

/// Follows main oscillator morph to morph between Sine/Spike/Saw wavetables
void FrequencyShifter::oscMorph(std::atomic<float>* morph)
{
    sineLevel  = oscParamControl.sinMorphGain(morph);
    spikeLevel = oscParamControl.spikeMorphGain(morph);
    sawLevel   = oscParamControl.sawMorphGain(morph);
}

/// Does the frequency shifting and returns the sample value
float FrequencyShifter::process()
{
    return freqShiftProcess();
}

/// Sets sampleRate for member wavetables and populates them
void FrequencyShifter::setUpFreqShiftWavetables()
{
    wtSine.SetSampleRate  (sampleRate);
    wtSaw.SetSampleRate   (sampleRate);
    wtSpike.SetSampleRate (sampleRate);
    
    wtSine.PopulateWavetable();
    wtSaw.PopulateWavetable();
    wtSpike.PopulateWavetable();
}

/// sets playback frequency
void FrequencyShifter::setFreqShiftIncrement()
{
    wtSine.SetIncrement  (modFrequency);
    wtSaw.SetIncrement   (modFrequency);
    wtSpike.SetIncrement (modFrequency);
}

/// Does the frequency shifting and returns the sample value
float FrequencyShifter::freqShiftProcess()
{
    float sinVal   = wtSine .Process() * sineLevel;
    float spikeVal = wtSpike.Process() * spikeLevel;
    float sawVal   = wtSaw  .Process() * sawLevel;
    
    return ( sinVal + spikeVal + sawVal ) * 0.5f;
}



//==============================================================================
//==============================================================================


//
// Sample and Hold Distortion
//

/// Constructor
SampleAndHold::SampleAndHold() : oscSampleVal(0.0f), holdSampleVal(0.0f) {}

/// Sets sampleRate for member wavetables and populates them
void SampleAndHold::setSampleRate(float SR)
{
    sampleRate = SR;
    setSampHoldWavetables();
}

/// sets playback frequency
void SampleAndHold::modFreq(float fqncy, std::atomic<float>* offset)
{
    modFrequency = fqncy * *offset;
    setSampHoldIncrement();
}

/**
Takes in current oscillator sample value and processes through S&H
*/
float SampleAndHold::processSH(float oscSampleValIn)
{
    oscSampleVal = oscSampleValIn;
    return sampleHoldProcess();
}

/// Sets sampleRate for member wavetables and populates them
void SampleAndHold::setSampHoldWavetables()
{
    wtSampHold.SetSampleRate(sampleRate);
    wtSampHold.PopulateWavetable();
}

/// sets playback frequency
void SampleAndHold::setSampHoldIncrement()
{
    wtSampHold.SetIncrement(modFrequency);
}

/// Processes S&H distortion and returns wave value
float SampleAndHold::sampleHoldProcess()
{
    float outVal;
    float sampHoldVal = wtSampHold.Process();
    
    if (sampHoldVal >= 0.0f)
    {
        outVal        = oscSampleVal;
        holdSampleVal = outVal;
    }
    else
    {
        outVal = holdSampleVal;
    }
        
    return outVal;
}
