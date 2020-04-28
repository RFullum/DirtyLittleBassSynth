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
    std::make_unique<AudioParameterFloat>("osc_morph", "Osc Morph", 0.0f, 2.0f, 0.0f),
    std::make_unique<AudioParameterFloat>("sub_osc_morph", "Sub Morph", 0.0f, 2.0f, 0.0f)
})

// CONSTRUCTOR!
{
    // Parameter construction
    oscMorphParameter = parameters.getRawParameterValue("osc_morph");
    subOscMorphParameter = parameters.getRawParameterValue("sub_osc_morph");
    
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
        v->setOscParamPointers(oscMorphParameter, subOscMorphParameter);
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
