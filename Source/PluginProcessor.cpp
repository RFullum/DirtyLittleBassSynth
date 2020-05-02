/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Wavetable5AudioProcessor::Wavetable5AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
parameters(*this, nullptr, "ParameterTree", {
    // id, description, min, max, default
    // Main Osc Params
    std::make_unique<AudioParameterFloat>("osc_morph", "Osc Morph", 0.0f, 2.0f, 0.0f),
    std::make_unique<AudioParameterFloat>("sub_osc_morph", "Sub Morph", 0.0f, 2.0f, 0.0f),
    std::make_unique<AudioParameterFloat>("sub_osc_gain", "Sub Gain", 0.0f, 1.0f, 0.0f),
    std::make_unique<AudioParameterFloat>("sub_osc_octave", "Sub Octave", 1.0f, 3.0f, 1.0f),
    
    // Amp ADSR Params
    std::make_unique<AudioParameterFloat>("amp_attack", "Amp Attack", 0.001f, 4.0f, 0.1f),
    std::make_unique<AudioParameterFloat>("amp_decay", "Amp Decay", 0.001f, 4.0f, 1.0f),
    std::make_unique<AudioParameterFloat>("amp_sustain", "Amp Sustain", 0.0f, 1.0f, 0.75f),
    std::make_unique<AudioParameterFloat>("amp_release", "Amp Release", 0.001f, 4.0f, 0.1f),
    
    // Foldback Distortion Params
    std::make_unique<AudioParameterFloat>("foldback_dist", "Foldback Distortion", 1.0f, 200.0f, 1.0f),
    
    // Ring Modulator Params
    std::make_unique<AudioParameterFloat>("ring_mod_pitch", "Ring Mod Pitch", 0.25f, 4.0f, 1.0f),
    std::make_unique<AudioParameterFloat>("ring_tone", "Ring Mod Tone", 0.0f, 1.0f, 0.0f),
    std::make_unique<AudioParameterFloat>("ring_mod_mix", "Ring Mod Mix", 0.0f, 1.0f, 0.0f),
    
    // Frequency Shifter Params
    std::make_unique<AudioParameterFloat>("freq_shift_pitch", "Freq Shift Pitch", 0.25f, 4.0f, 1.0f),
    std::make_unique<AudioParameterFloat>("freq_shift_mix", "Freq Shift Mix", 0.0f, 1.0f, 0.0f),
    
    // Sample and Hold Params
    std::make_unique<AudioParameterFloat>("sandh_pitch", "S&H Pitch", 1.0f, 128.0f, 128.0f),
    std::make_unique<AudioParameterFloat>("sandh_mix", "S&H Mix", 0.0f, 1.0f, 0.0f),
    
    // Filter Params
    std::make_unique<AudioParameterFloat>("filter_cutoff", "Filter Cutoff", 1.0f, 100.0f, 100.0f),
    std::make_unique<AudioParameterFloat>("filter_res", "Filter Resonance", 1.0f, 2.0f, 0.0f),
    std::make_unique<AudioParameterFloat>("filter_type", "-12LPF:0, -24LPF:1, -48LPF:2, Notch:3", 0.0f, 3.0f, 0.0f),
    
    // Filter Env Params
    std::make_unique<AudioParameterFloat>("filtEnv_attack", "Filter Attack", 0.001f, 4.0f, 0.01f),
    std::make_unique<AudioParameterFloat>("filtEnv_decay", "Filter Decay", 0.001f, 4.0f, 1.0f),
    std::make_unique<AudioParameterFloat>("filtEnv_sustain", "Filter Sustain", 0.0f, 1.0f, 0.75f),
    std::make_unique<AudioParameterFloat>("filtEnv_release", "Filter Release", 0.001f, 4.0f, 0.1f),
    std::make_unique<AudioParameterFloat>("filtEnv_COAmt", "Filter Env to Cutoff", 0.0f, 1.0f, 0.0f),
    std::make_unique<AudioParameterFloat>("filtEnv_ResAmt", "Filter Env to Res", 0.0f, 1.0f, 0.0f)
})

// CONSTRUCTOR!
{
    // Osc Parameter Construction
    oscMorphParameter = parameters.getRawParameterValue("osc_morph");
    subOscMorphParameter = parameters.getRawParameterValue("sub_osc_morph");
    subGainParameter = parameters.getRawParameterValue("sub_osc_gain");
    subOctaveParameter = parameters.getRawParameterValue("sub_osc_octave");
    
    // Amp ADSR Parameter Construction
    ampAttackParameter = parameters.getRawParameterValue("amp_attack");
    ampDecayParameter = parameters.getRawParameterValue("amp_decay");
    ampSustainParameter = parameters.getRawParameterValue("amp_sustain");
    ampReleaseParameter = parameters.getRawParameterValue("amp_release");
    
    // Foldback Parameter Construction
    foldbackDistParameter = parameters.getRawParameterValue("foldback_dist");
    
    // RingMod Parameter Construction
    ringModPitchParameter = parameters.getRawParameterValue("ring_mod_pitch");
    ringToneParameter = parameters.getRawParameterValue("ring_tone");
    ringModMixParameter = parameters.getRawParameterValue("ring_mod_mix");
    
    // Frequency Shift Parameter Construction
    freqShiftPitchParameter = parameters.getRawParameterValue("freq_shift_pitch");
    freqShiftMixParameter = parameters.getRawParameterValue("freq_shift_mix");
    
    // S&H Parameter Construction
    sAndHPitchParameter = parameters.getRawParameterValue("sandh_pitch");
    sAndHMixParameter = parameters.getRawParameterValue("sandh_mix");
    
    // Filter Parameter Construction
    filterCutoffParameter = parameters.getRawParameterValue("filter_cutoff");
    filterResonanceParameter = parameters.getRawParameterValue("filter_res");
    filterSelectorParameter = parameters.getRawParameterValue("filter_type");
    
    // Filter ADSR Parameter Construction
    filtEnvAttackParameter = parameters.getRawParameterValue("filtEnv_attack");
    filtEnvDecayParameter = parameters.getRawParameterValue("filtEnv_decay");
    filtEnvSustainParameter = parameters.getRawParameterValue("filtEnv_sustain");
    filtEnvReleaseParameter = parameters.getRawParameterValue("filtEnv_release");
    filtEnvAmtCOParameter = parameters.getRawParameterValue("filtEnv_COAmt");
    filtEnvAmtResParameter = parameters.getRawParameterValue("filtEnv_ResAmt");
    
    // Create Voices for each voice count
    for (int i=0; i<voiceCount; i++)
    {
        synth.addVoice( new MySynthVoice() );
    }
    
    // Add sound for synth class
    synth.addSound( new MySynthSound() );
    
    // Set Parameter Pointers for each voice
    for (int i=0; i<voiceCount; i++)
    {
        MySynthVoice* v = dynamic_cast<MySynthVoice*>(synth.getVoice(i));
        v->setOscParamPointers(oscMorphParameter, subOscMorphParameter, subGainParameter, subOctaveParameter);
        v->setAmpADSRParamPointers(ampAttackParameter, ampDecayParameter, ampSustainParameter, ampReleaseParameter);
        v->setDistParamPointers(foldbackDistParameter);
        v->setRingModParamPointers(ringModPitchParameter, ringToneParameter, ringModMixParameter);
        v->setFreqShiftParamPointers(freqShiftPitchParameter, freqShiftMixParameter);
        v->setSampleAndHoldParamPointers(sAndHPitchParameter, sAndHMixParameter);
        v->setFilterParamPointers(filterCutoffParameter, filterResonanceParameter, filterSelectorParameter);
        v->setFilterADSRParamPointers(filtEnvAttackParameter, filtEnvDecayParameter, filtEnvSustainParameter, filtEnvReleaseParameter, filtEnvAmtCOParameter, filtEnvAmtResParameter);
    }
    
}

Wavetable5AudioProcessor::~Wavetable5AudioProcessor()
{
}

//==============================================================================
const String Wavetable5AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Wavetable5AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Wavetable5AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Wavetable5AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Wavetable5AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Wavetable5AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Wavetable5AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Wavetable5AudioProcessor::setCurrentProgram (int index)
{
}

const String Wavetable5AudioProcessor::getProgramName (int index)
{
    return {};
}

void Wavetable5AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Wavetable5AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i=0; i<voiceCount; i++)
    {
        MySynthVoice* v = dynamic_cast<MySynthVoice*>(synth.getVoice(i));
        v->init(sampleRate);
    }
}

void Wavetable5AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Wavetable5AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Wavetable5AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
        
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool Wavetable5AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Wavetable5AudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void Wavetable5AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Wavetable5AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Wavetable5AudioProcessor();
}
