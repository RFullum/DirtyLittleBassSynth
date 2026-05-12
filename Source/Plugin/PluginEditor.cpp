#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

namespace
{
    GuiResources MakeResources(DirtyLittleBassSynthAudioProcessor &processor
                               , juce::LookAndFeel                &dialLAF
                               , juce::LookAndFeel                &dryWetLAF)
    {
        return GuiResources
                {
                    .apvts             = &processor.parameters,
                    .dialLookAndFeel   = &dialLAF,
                    .dryWetLookAndFeel = &dryWetLAF,
                    .theme             = Palette::DefaultTheme,
                    .scopeBuffer       = &processor.GetScopeBuffer(),
                    .tempoSnapshot     = &processor.GetTempoSnapshot(),
                    .isStandalone      = (processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone),
                    .midiLearnManager  = &processor.GetMidiLearnManager(),
                    .patchManager      = &processor.GetPatchManager(),
                };
    }
}

//==============================================================================

DirtyLittleBassSynthAudioProcessorEditor::DirtyLittleBassSynthAudioProcessorEditor(DirtyLittleBassSynthAudioProcessor &p)
: juce::AudioProcessorEditor(&p)
, processor(p)
, resources(MakeResources(p, dialLookAndFeel, dryWetLookAndFeel))
, titleHeader    (resources)
, sourcesColumn  (resources)
, filterColumn   (resources)
, modifiersColumn(resources)
, masterColumn   (resources)
, midiLearnOverlay(processor.GetMidiLearnManager(), resources.theme)
{
    setSize(1300, 700);

    dialLookAndFeel  .SetTrackBackground(resources.theme.structure);
    dryWetLookAndFeel.SetTrackBackground(resources.theme.structure);

    titleFooter.setTheme(resources.theme);

    addAndMakeVisible(titleHeader);
    addAndMakeVisible(titleFooter);
    addAndMakeVisible(sourcesColumn);
    addAndMakeVisible(filterColumn);
    addAndMakeVisible(modifiersColumn);
    addAndMakeVisible(masterColumn);
    
    addChildComponent(midiLearnOverlay);
    
    addKeyListener(this);
    setWantsKeyboardFocus(true);

    juce::Timer::startTimerHz(60);
}

DirtyLittleBassSynthAudioProcessorEditor::~DirtyLittleBassSynthAudioProcessorEditor()
{
    juce::Timer::stopTimer();
}

void DirtyLittleBassSynthAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(resources.theme.background);

    g.setColour(resources.theme.structure);
    for (const auto &d : dividers)
        g.fillRect(d);
}

void DirtyLittleBassSynthAudioProcessorEditor::resized()
{
    static constexpr int dividerThick = 1;
    static constexpr int dividerInset = 8;
    dividers.clear();
    
    auto bounds = getLocalBounds();
    
    titleHeader.setBounds(bounds.removeFromTop(66));
    titleFooter.setBounds(bounds.removeFromBottom(17));
    dividers.emplace_back(bounds.removeFromTop(dividerThick));
    dividers.emplace_back(bounds.removeFromBottom(dividerThick));

    midiLearnOverlay.setBounds(bounds);

    // 12 Slice Design: 4 columns of the synth spaced across the 12 column slices.
    const int bodyW       = bounds.getWidth();
    const int columnsSize = bodyW - (dividerThick * 3);
    const int sliceW      = columnsSize / 12;

    sourcesColumn  .setBounds   (bounds.removeFromLeft(sliceW * 4));
    dividers       .emplace_back(bounds.removeFromLeft(dividerThick).reduced(0, dividerInset));
    modifiersColumn.setBounds   (bounds.removeFromLeft(sliceW * 2));
    dividers       .emplace_back(bounds.removeFromLeft(dividerThick).reduced(0, dividerInset));
    filterColumn   .setBounds   (bounds.removeFromLeft(sliceW * 3));
    dividers       .emplace_back(bounds.removeFromLeft(dividerThick).reduced(0, dividerInset));
    masterColumn   .setBounds   (bounds.removeFromLeft(sliceW * 3));
}

bool DirtyLittleBassSynthAudioProcessorEditor::keyPressed(const juce::KeyPress &key, juce::Component *)
{
    // Standalone-only: in a DAW the host intercepts
    if (! resources.isStandalone)
        return false;

    // commandModifier maps to Cmd on macOS and Ctrl on Windows/Linux
    static const juce::KeyPress cmdS      ('s'
                                           , juce::ModifierKeys::commandModifier
                                           , 0);
    static const juce::KeyPress cmdShiftS ('s'
                                           , juce::ModifierKeys::commandModifier
                                                | juce::ModifierKeys::shiftModifier
                                           , 0);

    if (key == cmdShiftS)
    {
        titleHeader.TriggerPatchSaveAs();
        return true;
    }

    if (key == cmdS)
    {
        titleHeader.TriggerPatchSave();
        return true;
    }

    return false;
}

void DirtyLittleBassSynthAudioProcessorEditor::timerCallback()
{
    // Toggle the MIDI Learn overlay's visibility based on the manager's state.
    // The audio thread can transition Armed → Listening when a CC binds, so
    // polling here keeps the visual in sync without a dedicated listener.
    const auto learnState = processor.GetMidiLearnManager().GetState();
    const bool learning   = (learnState != MidiLearnManager::State::Idle);

    if (midiLearnOverlay.isVisible() != learning)
        midiLearnOverlay.setVisible(learning);

    if (learning || midiLearnOverlay.isVisible())
        midiLearnOverlay.Update();

    // Persist mappings whenever the manager flags itself dirty (binding landed,
    // unmap, clear-all). File I/O happens here on the message thread; the
    // audio thread only flips the atomic flag.
    if (processor.GetMidiLearnManager().IsDirty())
        processor.SaveMidiLearnMappings();

    titleHeader  .Update();
    sourcesColumn.Update();
    filterColumn .Update();

    auto      &lvl       = processor.outputLevelBuffer;
    const int  n         = lvl.getNumSamples();
    const int  numChans  = lvl.getNumChannels();

    float leftMag  = (numChans > 0 && n > 0) ? lvl.getMagnitude(0, 0, n) : 0.0f;
    float rightMag = (numChans > 1 && n > 0) ? lvl.getMagnitude(1, 0, n) : leftMag;

    if (leftMag  < 0.001f) leftMag  = 0.0f;
    if (rightMag < 0.001f) rightMag = 0.0f;

    const float grDb = processor.GetMasterGainReductionDb();
    masterColumn.Update(leftMag, rightMag, grDb, (float)processor.getSampleRate());
}
