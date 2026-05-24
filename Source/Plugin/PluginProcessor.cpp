#include "PluginProcessor.h"
#include "PluginEditor.h"

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
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"osc_morph",        1}, "Osc Morph",  juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("Morph")),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"sub_osc_morph",    1}, "Sub Morph",  juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("Morph")),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"sub_osc_gain",     1}, "Sub Gain",   juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("gain")),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"pitch_bend_range", 1}, "Pitch Bend", juce::NormalisableRange<float>(0.0f, 24.0f, 1.0f, 1.0f, false), 12.0f, juce::AudioParameterFloatAttributes().withLabel("semitones")),
                    std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"sub_osc_octave",   1}, "Sub Octave", juce::StringArray( {"0", "-1 Oct", "-2 Oct"} ), 0 ),
                    
                    // Amp ADSR Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_attack",  1}, "Amp Attack",  juce::NormalisableRange<float>(0.01f, 4.0f, 0.001f, 0.325f, false), 0.1f, juce::AudioParameterFloatAttributes().withLabel("attack")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_decay",   1}, "Amp Decay",   juce::NormalisableRange<float>(0.01f, 4.0f, 0.01f,  0.325f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("decay")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_sustain", 1}, "Amp Sustain", juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  1.0f,   false), 0.75f, juce::AudioParameterFloatAttributes().withLabel("sustain level")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp_release", 1}, "Amp Release", juce::NormalisableRange<float>(0.01f, 4.0f, 0.01f,  0.325f, false), 0.1f, juce::AudioParameterFloatAttributes().withLabel("release")),
                    
                    // Portament Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"porta_time", 1}, "Portamento Time", juce::NormalisableRange<float>(0.01f, 1.0f, 0.0f, 0.325f, false), 0.02f, juce::AudioParameterFloatAttributes().withLabel("portamento")),
                    std::make_unique<juce::AudioParameterBool> (juce::ParameterID{"porta_on",     1}, "Portamento On",     false),
                    std::make_unique<juce::AudioParameterBool> (juce::ParameterID{"porta_legato", 1}, "Portamento Legato", false),
                    
                    // Foldback Distortion Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"foldback_dist", 1}, "Foldback Distortion", juce::NormalisableRange<float>(1.0f, 200.0f, 0.00f, 0.325f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("foldback")),
                    
                    // Ring Modulator Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ring_mod_pitch", 1}, "Ring Mod Pitch", juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 1.0f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("ring pitch")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ring_tone",      1}, "Ring Mod Tone",  juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("ring tone")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ring_mod_mix",   1}, "Ring Mod Mix",   juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("ring dry/wet")),
                    
                    // Frequency Shifter Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"freq_shift_pitch", 1}, "Freq Shift Pitch", juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 1.0f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("freq shift pitch")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"freq_shift_mix",   1}, "Freq Shift Mix",   juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("freq shift dry/wet")),
                    
                    // Sample and Hold Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"sandh_pitch", 1}, "S&H Pitch", juce::NormalisableRange<float>(1.0f, 128.0f, 0.01f, 0.25f, false), 128.0f, juce::AudioParameterFloatAttributes().withLabel("s&h pitch")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"sandh_mix",   1}, "S&H Mix",   juce::NormalisableRange<float>(0.0f, 1.0f,   0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("s&h dry/wet")),
                    
                    // Filter Params
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"filter_cutoff", 1}, "Filter Cutoff",    juce::NormalisableRange<float>(0.0f, 1.0f,   0.001f, 1.0f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("cutoff")),
                    std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"filter_res",    1}, "Filter Resonance", juce::NormalisableRange<float>(1.0f, 2.0f,   0.01f,  1.0f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("resonance")),
                    std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"filter_type",   1}, "Filter Type",      juce::StringArray( {"-12LPF", "-24LPF", "-48LPF", "Notch"} ), 0 ),
                    
                    // Filter Env Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_attack",  1}, "Filter Attack",        juce::NormalisableRange<float>(0.01f, 4.0f, 0.001f, 0.325f, false), 0.01f, juce::AudioParameterFloatAttributes().withLabel("attack")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_decay",   1}, "Filter Decay",         juce::NormalisableRange<float>(0.1f,  4.0f, 0.01f,  0.325f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("decay")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_sustain", 1}, "Filter Sustain",       juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  1.0f,   false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("sustain level")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_release", 1}, "Filter Release",       juce::NormalisableRange<float>(0.01f, 4.0f, 0.01f,  0.325f, false), 0.1f, juce::AudioParameterFloatAttributes().withLabel("release")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_COAmt",   1}, "Filter Env to Cutoff", juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  1.0f,   false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("env to cutoff")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtEnv_ResAmt",  1}, "Filter Env to Res",    juce::NormalisableRange<float>(0.0f,  1.0f, 0.01f,  1.0f,   false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("env to resonance")),
                    
                    // Filter LFO Params
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtLFO_freq",  1}, "Filter LFO Freq",   juce::NormalisableRange<float>(0.01f, 15.0f, 0.01f, 0.6f, false), 1.0f, juce::AudioParameterFloatAttributes().withLabel("lfo freq")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtLFO_amt",   1}, "Filter LFO Amount", juce::NormalisableRange<float>(0.0f,  1.0f,  0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("lfo amt")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"filtLFO_shape", 1}, "Filter LFO Shape",  juce::NormalisableRange<float>(0.0f,  2.0f,  0.01f, 1.0f, false), 0.0f, juce::AudioParameterFloatAttributes().withLabel("lfo shape")),
                    
                    // Master Gain
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"master_gain", 1}, "Master Gain", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 2.0f, true), 1.0f, juce::AudioParameterFloatAttributes().withLabel("master gain") ),

                    // Master Stereo (Haas widener + bass mono-izer crossover)
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"master_wide",     1}, "Stereo Width",    juce::NormalisableRange<float>(-1.0f,    1.0f, 0.001f, 1.0f, false),   0.0f, juce::AudioParameterFloatAttributes().withLabel("widen")),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"mono_below_freq", 1}, "Mono Below Freq", juce::NormalisableRange<float>(20.0f, 300.0f, 1.0f,   0.5f, false), 120.0f, juce::AudioParameterFloatAttributes().withLabel("mono crossover")),

                    // Master Limiter
                    std::make_unique<juce::AudioParameterBool> (juce::ParameterID{"limiter_on",      1}, "Limiter On",     true),
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"limiter_ceiling", 1}, "Limiter Ceiling", juce::NormalisableRange<float>(-12.0f, 0.0f, 0.1f, 1.0f, false), -0.1f, juce::AudioParameterFloatAttributes().withLabel("ceiling")),

                    // Tempo (fallback when no host transport / standalone)
                    std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"tempo_fallback_bpm", 1}, "Tempo (Fallback)", juce::NormalisableRange<float>(30.0f, 300.0f, 0.1f, 1.0f, false), 120.0f, juce::AudioParameterFloatAttributes().withLabel("BPM")),

                    // LFO sync mode (placeholder — DSP wired in Step 7).
                    std::make_unique<juce::AudioParameterBool>  (juce::ParameterID{"filtLFO_sync",     1}, "LFO Sync", false),
                    std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"filtLFO_sync_div", 1}, "LFO Sync Subdivision",
                                                                 juce::StringArray({"1/1", "1/2", "1/4", "1/4D", "1/4T",
                                                                                    "1/8", "1/8D", "1/8T",
                                                                                    "1/16", "1/16D", "1/16T", "1/32"}),
                                                                 5)   // default index 5 = "1/8"
                })
{
    oscMorphParameter    = parameters.getRawParameterValue("osc_morph");
    subOscMorphParameter = parameters.getRawParameterValue("sub_osc_morph");
    subGainParameter     = parameters.getRawParameterValue("sub_osc_gain");
    subOctaveParameter   = parameters.getRawParameterValue("sub_osc_octave");
    portaTimeParameter   = parameters.getRawParameterValue("porta_time");
    portaOnParameter     = parameters.getRawParameterValue("porta_on");
    portaLegatoParameter = parameters.getRawParameterValue("porta_legato");
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

    filtLFOSyncOnParameter  = parameters.getRawParameterValue("filtLFO_sync");
    filtLFOSyncDivParameter = parameters.getRawParameterValue("filtLFO_sync_div");
    
    masterGainParameter = parameters.getRawParameterValue("master_gain");

    masterWideParameter    = parameters.getRawParameterValue("master_wide");
    monoBelowFreqParameter = parameters.getRawParameterValue("mono_below_freq");

    limiterOnParameter      = parameters.getRawParameterValue("limiter_on");
    limiterCeilingParameter = parameters.getRawParameterValue("limiter_ceiling");

    tempoFallbackBpmParameter = parameters.getRawParameterValue("tempo_fallback_bpm");

    masterChain.SetParamPointers       (masterWideParameter, monoBelowFreqParameter);
    masterChain.SetLimiterParamPointers(limiterOnParameter,  limiterCeilingParameter);

    // Create voices and cache typed pointers (synth owns them for our lifetime).
    typedVoices.reserve(voiceCount);
    for (int i = 0; i < voiceCount; ++i)
    {
        auto* voice = new BassSynthVoice();
        typedVoices.push_back(voice);
        synth.addVoice(voice);
    }

    synth.addSound(new BassSynthSound());

    for (auto* v : typedVoices)
    {
        v->SetOscParamPointers             (oscMorphParameter, subOscMorphParameter, subGainParameter, subOctaveParameter);
        v->SetAmpADSRParamPointers         (ampAttackParameter, ampDecayParameter, ampSustainParameter, ampReleaseParameter);
        v->SetPortamentoParamPointers      (portaTimeParameter);
        v->SetPortamentoModeParamPointers  (portaOnParameter, portaLegatoParameter);
        v->SetDistParamPointers            (foldbackDistParameter);
        v->SetRingModParamPointers         (ringModPitchParameter, ringToneParameter, ringModMixParameter);
        v->SetFreqShiftParamPointers       (freqShiftPitchParameter, freqShiftMixParameter);
        v->SetSampleAndHoldParamPointers   (sAndHPitchParameter, sAndHMixParameter);
        v->SetFilterParamPointers          (filterCutoffParameter, filterResonanceParameter, filterSelectorParameter);
        v->SetFilterADSRParamPointers      (filtEnvAttackParameter, filtEnvDecayParameter, filtEnvSustainParameter, filtEnvReleaseParameter, filtEnvAmtCOParameter, filtEnvAmtResParameter);
        v->SetFilterLFOParamPointers       (filtLFOFreqParameter, filtLFOAmtParameter, filtLFOShapeParameter);
        v->SetFilterLFOSyncParamPointers   (filtLFOSyncOnParameter, filtLFOSyncDivParameter);
        v->SetFilterDisplaySnapshotPointers(&filtEnvDisplayValue, &filtLFODisplayValue);
        v->SetTempoSnapshot                (&tempoSnapshot);
        v->SetMasterGainParamPointers      (masterGainParameter);
        v->updatePitchBendRange            (*pitchBendParameter);
    }

    // === MIDI Learn ===
    juce::PropertiesFile::Options propsOptions;
    propsOptions.applicationName     = "DirtyLittleBassSynth";
    propsOptions.filenameSuffix      = ".settings";
    propsOptions.folderName          = "FullumMusic";
    propsOptions.osxLibrarySubFolder = "Application Support";
    applicationProperties.setStorageParameters(propsOptions);

    RegisterMidiLearnableParams();
    LoadMidiLearnMappings();

    for (auto &slot : pendingCcEcho)
        slot.store(-1, std::memory_order_relaxed);

    if (auto *userSettings = applicationProperties.getUserSettings())
        ccEchoEnabled.store(userSettings->getBoolValue("ccEchoEnabled", true), std::memory_order_relaxed);

    AttachCcEchoListeners();

    patchManager.Init();
}

void DirtyLittleBassSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (auto* v : typedVoices)
        v->Init(sampleRate, samplesPerBlock);

    masterChain.Prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    masterChain.Reset();

    scopeBuffer.Clear();

    // Voices share the same oversampling latency (identical config). Report the
    // first voice's latency to the host so it can compensate.
    const int oversamplingLatency = typedVoices.empty()
                                        ? 0
                                        : typedVoices.front()->GetOversamplingLatencyInSamples();

    setLatencySamples(oversamplingLatency);

    DBG("DLBS: oversampling latency = "
        << oversamplingLatency
        << " samples ("
        << juce::String(1000.0 * (double) oversamplingLatency / sampleRate, 3)
        << " ms at "
        << juce::String(sampleRate, 0) << " Hz)");
}

void DirtyLittleBassSynthAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DirtyLittleBassSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DirtyLittleBassSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // === MIDI CC routing (learn mappings + standalone defaults) ===
    // The manager owns all CC→param dispatch. It also performs the binding when
    // the user is in Learn mode and a param has been armed.
    for (const auto meta : midiMessages)
    {
        const auto m = meta.getMessage();

        if (m.isController())
            midiLearnManager.HandleControllerMessage(m.getControllerNumber()
                                                     , m.getControllerValue());
    }

    for (int cc = 0; cc < (int) pendingCcEcho.size(); ++cc)
    {
        const int pending = pendingCcEcho[(size_t) cc].exchange(-1, std::memory_order_acq_rel);
        if (pending >= 0)
            midiMessages.addEvent(juce::MidiMessage::controllerEvent(1, cc, pending), 0);
    }

    {
        TempoInfo info;
        info.bpm                = (tempoFallbackBpmParameter != nullptr)
                                      ? tempoFallbackBpmParameter->load()
                                      : 120.0f;
        info.bpmFromHost        = false;
        info.timeSigNumerator   = 4;
        info.timeSigDenominator = 4;
        info.isPlaying          = false;
        info.ppqPosition        = 0.0f;

        if (auto *playHead = getPlayHead())
        {
            if (auto position = playHead->getPosition())
            {
                if (auto hostBpm = position->getBpm())
                {
                    info.bpm         = (float) *hostBpm;
                    info.bpmFromHost = true;
                }

                if (auto ppq = position->getPpqPosition())
                    info.ppqPosition = (float) *ppq;

                info.isPlaying = position->getIsPlaying();

                if (auto timeSig = position->getTimeSignature())
                {
                    info.timeSigNumerator   = timeSig->numerator;
                    info.timeSigDenominator = timeSig->denominator;
                }
            }
        }

        tempoSnapshot.Update(info);
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    for (auto* v : typedVoices)
        v->updatePitchBendRange(*pitchBendParameter);

    masterChain.Process(buffer);

    scopeBuffer.PushBlock(buffer);

    outputLevelBuffer.clear();
    outputLevelBuffer = buffer;
}

juce::AudioProcessorEditor* DirtyLittleBassSynthAudioProcessor::createEditor()
{
    return new DirtyLittleBassSynthAudioProcessorEditor(*this);
}

bool DirtyLittleBassSynthAudioProcessor::hasEditor() const
{
    return true;
}

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
    // Some hosts don't cope well with 0 programs.
    return 1;
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

void DirtyLittleBassSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Wrap the APVTS state in a DLBSPluginState root so we can also stash the
    // currently-loaded patch path. Reopening a host project restores both the
    // exact param values AND the patch name shown in the title header.
    juce::ValueTree root { "DLBSPluginState" };
    root.setProperty("currentPatchPath"
                     , patchManager.GetCurrentPatchFile().getFullPathName()
                     , nullptr);
    root.appendChild(parameters.copyState(), nullptr);

    std::unique_ptr<juce::XmlElement> xml(root.createXml());
    copyXmlToBinary(*xml, destData);
}

void DirtyLittleBassSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState == nullptr)
        return;

    // New format: <DLBSPluginState currentPatchPath="…"> wrapping the APVTS
    // state as a child. Read both halves.
    if (xmlState->hasTagName("DLBSPluginState"))
    {
        const auto root       = juce::ValueTree::fromXml(*xmlState);
        const auto apvtsChild = root.getChildWithName(parameters.state.getType());

        if (apvtsChild.isValid())
            parameters.replaceState(apvtsChild);

        // Patch path is best-effort — missing file or empty string just leaves
        // current-patch tracking on Init (per SetCurrentFromRestoredPath).
        const auto pathString = root.getProperty("currentPatchPath").toString();
        if (pathString.isNotEmpty())
            patchManager.SetCurrentFromRestoredPath(juce::File(pathString));

        return;
    }

    // Old format: bare <ParameterTree>…</> — restore as before for any
    // sessions saved before the wrapper landed.
    if (xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void DirtyLittleBassSynthAudioProcessor::SaveMidiLearnMappings()
{
    if (auto *userSettings = applicationProperties.getUserSettings())
    {
        userSettings->setValue("midiLearnMappings", midiLearnManager.SerialiseMappings());
        userSettings->saveIfNeeded();
    }

    midiLearnManager.ClearDirtyFlag();
}

void DirtyLittleBassSynthAudioProcessor::MidiPanic()
{
    // midiChannel = 0 stops every voice on every channel. allowTailOff = false
    // gives a hard stop so a stuck note can't sustain through an envelope tail.
    synth.allNotesOff(0, false);
}

DirtyLittleBassSynthAudioProcessor::~DirtyLittleBassSynthAudioProcessor()
{
    DetachCcEchoListeners();
}

bool DirtyLittleBassSynthAudioProcessor::GetCcEchoEnabled() const
{
    return ccEchoEnabled.load(std::memory_order_acquire);
}

void DirtyLittleBassSynthAudioProcessor::SetCcEchoEnabled(bool enabled)
{
    ccEchoEnabled.store(enabled, std::memory_order_release);

    if (auto *userSettings = applicationProperties.getUserSettings())
    {
        userSettings->setValue("ccEchoEnabled", enabled);
        userSettings->saveIfNeeded();
    }
}

void DirtyLittleBassSynthAudioProcessor::AttachCcEchoListeners()
{
    for (int i = 0; i < midiLearnManager.GetNumParams(); ++i)
    {
        const auto id = midiLearnManager.GetParamID(i);

        if (id.isNotEmpty())
        {
            parameters.addParameterListener(id, this);
            ccEchoListenedParamIDs.add(id);
        }
    }
}

void DirtyLittleBassSynthAudioProcessor::DetachCcEchoListeners()
{
    for (const auto &id : ccEchoListenedParamIDs)
        parameters.removeParameterListener(id, this);

    ccEchoListenedParamIDs.clear();
}

void DirtyLittleBassSynthAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    juce::ignoreUnused(newValue);

    // Standalone-only feature
    if (wrapperType != juce::AudioProcessor::wrapperType_Standalone)
        return;

    if (! ccEchoEnabled.load(std::memory_order_acquire))
        return;

    const int paramIndex = midiLearnManager.GetParamIndexById(parameterID);
    if (paramIndex < 0)
        return;

    // Suppress feedback
    if (midiLearnManager.ConsumeAppliedFromCcFlag(paramIndex))
        return;

    const int cc = midiLearnManager.GetFirstCcForParam(paramIndex);
    if (cc < 0)
        return;     // param has no CC mapping; nothing to echo

    auto *param = midiLearnManager.GetParam(paramIndex);
    if (param == nullptr)
        return;

    const float normalised = param->getValue();             // already 0..1
    const int   ccValue    = juce::jlimit(0, 127, (int) std::round(normalised * 127.0f));

    // Store the latest value; if multiple changes land before processBlock
    // drains, we send the most recent — which is what a controller wants.
    pendingCcEcho[(size_t) cc].store(ccValue, std::memory_order_release);
}

bool DirtyLittleBassSynthAudioProcessor::GetTooltipsEnabled() const
{
    // const_cast is fine here: getUserSettings() just lazily instantiates the
    // underlying PropertiesFile; reading the value is itself a const op.
    if (auto *userSettings = const_cast<juce::ApplicationProperties &>(applicationProperties).getUserSettings())
        return userSettings->getBoolValue("tooltipsEnabled", true);

    return true;
}

void DirtyLittleBassSynthAudioProcessor::SetTooltipsEnabled(bool enabled)
{
    if (auto *userSettings = applicationProperties.getUserSettings())
    {
        userSettings->setValue("tooltipsEnabled", enabled);
        userSettings->saveIfNeeded();
    }
}

void DirtyLittleBassSynthAudioProcessor::RegisterMidiLearnableParams()
{
    // Order here is the stable param-index order serialised mappings rely on.
    // Adding params later is fine; renaming or removing one will silently drop
    // existing mappings that referenced the old paramID (handled in
    // MidiLearnManager::RestoreMappings).
    static const juce::StringArray learnableIDs
    {
        "osc_morph", "sub_osc_morph", "sub_osc_gain", "sub_osc_octave",
        "amp_attack", "amp_decay", "amp_sustain", "amp_release",
        "porta_time", "porta_on", "porta_legato",
        "foldback_dist",
        "ring_mod_pitch", "ring_tone", "ring_mod_mix",
        "freq_shift_pitch", "freq_shift_mix",
        "sandh_pitch", "sandh_mix",
        "filter_cutoff", "filter_res", "filter_type",
        "filtEnv_attack", "filtEnv_decay", "filtEnv_sustain", "filtEnv_release",
        "filtEnv_COAmt", "filtEnv_ResAmt",
        "filtLFO_freq", "filtLFO_amt", "filtLFO_shape",
        "filtLFO_sync", "filtLFO_sync_div",
        "master_gain", "master_wide", "mono_below_freq",
        "limiter_on", "limiter_ceiling"
    };

    for (const auto &id : learnableIDs)
        midiLearnManager.RegisterParam(parameters, id);

    // Standalone-only default: CC1 (mod wheel) → Filter LFO Amount. Registered
    // as a default (not a one-shot mapping) so a Clear Maps action restores it
    // afterwards. Any user override loaded via LoadMidiLearnMappings() takes
    // priority since RestoreMappings runs after this default is applied.
    if (wrapperType == wrapperType_Standalone)
    {
        const int lfoAmtIdx = midiLearnManager.GetParamIndexById("filtLFO_amt");
        midiLearnManager.RegisterDefaultMapping(1, lfoAmtIdx);
    }
}

void DirtyLittleBassSynthAudioProcessor::LoadMidiLearnMappings()
{
    if (auto *userSettings = applicationProperties.getUserSettings())
    {
        const auto serialised = userSettings->getValue("midiLearnMappings", juce::String());
        if (serialised.isNotEmpty())
            midiLearnManager.RestoreMappings(serialised);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DirtyLittleBassSynthAudioProcessor();
}
