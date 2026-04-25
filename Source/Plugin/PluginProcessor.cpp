/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

//
// ParameterFloats:
// id, description, min, max, default
// ~OR~
// id, description, normalisableRange(min, max, increment, skew, symmetric),
//                 default, param label, param category, string from value, string to value
//
// ParameterChoices:
// id, descript, choices (juce::StringArray), default index of juce::StringArray
//

//==============================================================================

DirtyLittleBassSynthAudioProcessor::DirtyLittleBassSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
 : juce::AudioProcessor(juce::AudioProcessor::BusesProperties()
                 #if ! JucePlugin_IsMidiEffect
                  #if ! JucePlugin_IsSynth
                   .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                  #endif
                   .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                 #endif
                   )
#endif
, parameters(*this
             , nullptr
             , "ParameterTree"
             , {
                    // Osc Params
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"osc_morph",        1}, "Osc Morph",  juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 1.0f, false), 0.0f, "Morph"),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"sub_osc_morph",    1}, "Sub Morph",  juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 1.0f, false), 0.0f, "Morph"),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"sub_osc_gain",     1}, "Sub Gain",   juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 4.0f, false), 0.0f, "gain"),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"pitch_bend_range", 1}, "Pitch Bend", juce::NormalisableRange<float>(0.0f, 24.0f, 1.0f, 1.0f, false), 12.0f, "semitones"),
                    std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"sub_osc_octave",   1}, "Sub Octave", juce::StringArray( {"0", "-1 Oct", "-2 Oct"} ), 0 ),
                    
                    // Amp juce::ADSR Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_attack",  1}, "Amp Attack",  juce::NormalisableRange<float>(0.01f, 4.0f, 0.001f, 0.325f, false), 0.1f,  "attack"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_decay",   1}, "Amp Decay",   juce::NormalisableRange<float>(0.01f, 4.0f, 0.01f,  0.325f, false), 1.0f,  "decay"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_sustain", 1}, "Amp Sustain", juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  4.0f,   false), 0.75f, "sustain level"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_release", 1}, "Amp Release", juce::NormalisableRange<float>(0.01f, 4.0f, 0.01f,  0.325f, false), 0.1f,  "release"),
                    
                    // Portament Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"porta_time", 1}, "Portamento juce::Time", juce::NormalisableRange<float>(0.01f, 1.0f, 0.0f, 0.325f, false), 0.02f, "portamento"),
                    
                    // Foldback Distortion Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"foldback_dist", 1}, "Foldback Distortion", juce::NormalisableRange<float>(1.0f, 200.0f, 0.00f, 0.325f, false), 1.0f, "foldback"),
                    
                    // Ring Modulator Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ring_mod_pitch", 1}, "Ring Mod Pitch", juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 1.0f, false), 1.0f, "ring pitch"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ring_tone",      1}, "Ring Mod Tone",  juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f, 1.0f, false), 0.0f, "ring tone"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ring_mod_mix",   1}, "Ring Mod Mix",   juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f, 1.0f, false), 0.0f, "ring dry/wet"),
                    
                    // Frequency Shifter Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"freq_shift_pitch", 1}, "Freq Shift Pitch", juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 1.0f, false), 1.0f, "freq shift pitch"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"freq_shift_mix",   1}, "Freq Shift Mix",   juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f, 1.0f, false), 0.0f, "freq shift dry/wet"),
                    
                    // Sample and Hold Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"sandh_pitch", 1}, "S&H Pitch", juce::NormalisableRange<float>(1.0f, 128.0f, 0.01f, 1.0f, false), 128.0f, "s&h pitch"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"sandh_mix",   1}, "S&H Mix",   juce::NormalisableRange<float>(0.0f, 1.0f,   0.01f, 1.0f, false), 0.0f,   "s&h dry/wet"),
                    
                    // Filter Params
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"filter_cutoff", 1}, "Filter Cutoff",    juce::NormalisableRange<float>(1.1f, 100.0f, 0.01f, 0.25f, false), 100.0f, "cutoff"),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"filter_res",    1}, "Filter Resonance", juce::NormalisableRange<float>(1.0f, 2.0f,   0.01f, 1.0f,  false), 0.0f,   "resonance"),
                    std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"filter_type",   1}, "Filter Type",      juce::StringArray( {"-12LPF", "-24LPF", "-48LPF", "Notch"} ), 0 ),
                    
                    // Filter Env Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_attack",  1}, "Filter Attack",        juce::NormalisableRange<float>(0.01f, 4.0f, 0.001f, 0.325f, false), 0.01f, "attack"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_decay",   1}, "Filter Decay",         juce::NormalisableRange<float>(0.1f,  4.0f, 0.01f,  0.325f, false), 1.0f,  "decay"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_sustain", 1}, "Filter Sustain",       juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  4.0f,   false), 1.0f,  "sustain level"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_release", 1}, "Filter Release",       juce::NormalisableRange<float>(0.01f, 4.0f, 0.01f,  0.325f, false), 0.1f,  "release"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_COAmt",   1}, "Filter Env to Cutoff", juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  1.0f,   false), 0.0f,  "env to cutoff"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_ResAmt",  1}, "Filter Env to Res",    juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  1.0f,   false), 0.0f,  "env to resonance"),
                    
                    // Filter LFO Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtLFO_freq",  1}, "Filter LFO Freq",   juce::NormalisableRange<float>(0.01f, 15.0f, 0.01f, 1.0f, false), 1.0f, "lfo freq"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtLFO_amt",   1}, "Filter LFO Amount", juce::NormalisableRange<float>(0.0f,  1.0f,  0.01f, 1.0f, false), 0.0f, "lfo amt"),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtLFO_shape", 1}, "Filter LFO Shape",  juce::NormalisableRange<float>(0.0f,  2.0f,  0.01f, 1.0f, false), 0.0f, "lfo shape"),
                    
                    // Master Gain
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"master_gain", 1}, "Master Gain", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 2.0f, true), 1.0f, "master gain" )
                })
{
    oscMorphParameter    = parameters.getRawParameterValue("osc_morph");
    subOscMorphParameter = parameters.getRawParameterValue("sub_osc_morph");
    subGainParameter     = parameters.getRawParameterValue("sub_osc_gain");
    subOctaveParameter   = parameters.getRawParameterValue("sub_osc_octave");
    portaTimeParameter   = parameters.getRawParameterValue("porta_time");
    pitchBendParameter   = parameters.getRawParameterValue("pitch_bend_range");
    
    ampAttackParameter  = parameters.getRawParameterValue("amp_attack");
    ampDecayParameter   = parameters.getRawParameterValue("amp_decay");
    ampSustainParameter = parameters.getRawParameterValue("amp_sustain");
    ampReleaseParameter = parameters.getRawParameterValue("amp_release");
    
    foldbackDistParameter = parameters.getRawParameterValue("foldback_dist");
    
    ringModPitchParameter = parameters.getRawParameterValue("ring_mod_pitch");
    ringToneParameter     = parameters.getRawParameterValue("ring_tone");
    ringModMixParameter   = parameters.getRawParameterValue("ring_mod_mix");
    
    freqShiftPitchParameter = parameters.getRawParameterValue("freq_shift_pitch");
    freqShiftMixParameter   = parameters.getRawParameterValue("freq_shift_mix");
    
    sAndHPitchParameter = parameters.getRawParameterValue("sandh_pitch");
    sAndHMixParameter   = parameters.getRawParameterValue("sandh_mix");
    
    filterCutoffParameter    = parameters.getRawParameterValue("filter_cutoff");
    filterResonanceParameter = parameters.getRawParameterValue("filter_res");
    filterSelectorParameter  = parameters.getRawParameterValue("filter_type");
    
    filtEnvAttackParameter  = parameters.getRawParameterValue("filtEnv_attack");
    filtEnvDecayParameter   = parameters.getRawParameterValue("filtEnv_decay");
    filtEnvSustainParameter = parameters.getRawParameterValue("filtEnv_sustain");
    filtEnvReleaseParameter = parameters.getRawParameterValue("filtEnv_release");
    filtEnvAmtCOParameter   = parameters.getRawParameterValue("filtEnv_COAmt");
    filtEnvAmtResParameter  = parameters.getRawParameterValue("filtEnv_ResAmt");
    
    filtLFOFreqParameter  = parameters.getRawParameterValue("filtLFO_freq");
    filtLFOAmtParameter   = parameters.getRawParameterValue("filtLFO_amt");
    filtLFOShapeParameter = parameters.getRawParameterValue("filtLFO_shape");
    
    masterGainParameter = parameters.getRawParameterValue("master_gain");
    
    // Create voices and cache typed pointers (synth owns them for our lifetime).
    typedVoices.reserve(voiceCount);
    for (int i = 0; i < voiceCount; ++i)
    {
        auto* voice = new MySynthVoice();
        typedVoices.push_back(voice);
        synth.addVoice(voice);
    }

    synth.addSound( new MySynthSound() );

    // Set Parameter Pointers for each voice
    for (auto* v : typedVoices)
    {
        v->SetOscParamPointers          (oscMorphParameter, subOscMorphParameter, subGainParameter, subOctaveParameter);
        v->SetAmpADSRParamPointers      (ampAttackParameter, ampDecayParameter, ampSustainParameter, ampReleaseParameter);
        v->SetPortamentoParamPointers   (portaTimeParameter);
        v->SetDistParamPointers         (foldbackDistParameter);
        v->SetRingModParamPointers      (ringModPitchParameter, ringToneParameter, ringModMixParameter);
        v->SetFreqShiftParamPointers    (freqShiftPitchParameter, freqShiftMixParameter);
        v->SetSampleAndHoldParamPointers(sAndHPitchParameter, sAndHMixParameter);
        v->SetFilterParamPointers       (filterCutoffParameter, filterResonanceParameter, filterSelectorParameter);
        v->SetFilterADSRParamPointers   (filtEnvAttackParameter, filtEnvDecayParameter, filtEnvSustainParameter, filtEnvReleaseParameter, filtEnvAmtCOParameter, filtEnvAmtResParameter);
        v->SetFilterLFOParamPointers    (filtLFOFreqParameter, filtLFOAmtParameter, filtLFOShapeParameter);
        v->SetMasterGainParamPointers   (masterGainParameter);
        v->updatePitchBendRange         (*pitchBendParameter);
    }
}

DirtyLittleBassSynthAudioProcessor::~DirtyLittleBassSynthAudioProcessor() {}

const juce::String DirtyLittleBassSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DirtyLittleBassSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DirtyLittleBassSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DirtyLittleBassSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DirtyLittleBassSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DirtyLittleBassSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DirtyLittleBassSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DirtyLittleBassSynthAudioProcessor::setCurrentProgram(int /*index*/) {}

const juce::String DirtyLittleBassSynthAudioProcessor::getProgramName(int /*index*/)
{
    return {};
}

void DirtyLittleBassSynthAudioProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/) {}

void DirtyLittleBassSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (auto* v : typedVoices)
        v->Init(sampleRate, samplesPerBlock);
}

void DirtyLittleBassSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DirtyLittleBassSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void DirtyLittleBassSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Hand off DSP to juce::Synthesiser class
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    for (auto* v : typedVoices)
        v->updatePitchBendRange(*pitchBendParameter);
    
    // Transport info
    //updateCurrentTimeInfoFromHost();
    
    // Level Metering
    outputLevelBuffer.clear();
    outputLevelBuffer = buffer;
}

bool DirtyLittleBassSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DirtyLittleBassSynthAudioProcessor::createEditor()
{
    //return new GenericAudioProcessorEditor (*this);
    return new DirtyLittleBassSynthAudioProcessorEditor(*this);
}

void DirtyLittleBassSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // getStateInformation
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void DirtyLittleBassSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // setStateInformation
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
}

//==============================================================================
/*
void DirtyLittleBassSynthAudioProcessor::updateCurrentTimeInfoFromHost()
{
    const auto newInfo = [&]
    {
        if (auto* ph = getPlayHead())
        {
            
            
            juce::AudioPlayHead::CurrentPositionInfo result;

            if (ph->getCurrentPosition (result))
                return result;
        }

        // If the host fails to provide the current time, we'll just use default values
        juce::AudioPlayHead::CurrentPositionInfo result;
        result.resetToDefault();
        return result;
    }();

    playHeadInfo.bpm = newInfo.bpm;
}
*/

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DirtyLittleBassSynthAudioProcessor();
}
