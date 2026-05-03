#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

namespace
{
    GuiResources MakeResources(DirtyLittleBassSynthAudioProcessor &processor
                               , juce::LookAndFeel                &dialLAF
                               , juce::LookAndFeel                &dryWetLAF)
    {
        return GuiResources{
            .apvts             = &processor.parameters,
            .dialLookAndFeel   = &dialLAF,
            .dryWetLookAndFeel = &dryWetLAF,
            .theme             = Palette::DefaultTheme,
            .scopeBuffer       = &processor.GetScopeBuffer(),
        };
    }
}

//==============================================================================

DirtyLittleBassSynthAudioProcessorEditor::DirtyLittleBassSynthAudioProcessorEditor(DirtyLittleBassSynthAudioProcessor &p)
: juce::AudioProcessorEditor(&p)
, processor(p)
, resources(MakeResources(p, dialLookAndFeel, dryWetLookAndFeel))
, sourcesColumn  (resources)
, filterColumn   (resources)
, modifiersColumn(resources)
, masterColumn   (resources)
{
    setSize(1300, 700);

    dialLookAndFeel  .SetTrackBackground(resources.theme.structure);
    dryWetLookAndFeel.SetTrackBackground(resources.theme.structure);

    titleHeader.SetTheme(resources.theme);
    titleFooter.setTheme(resources.theme);

    addAndMakeVisible(titleHeader);
    addAndMakeVisible(titleFooter);
    addAndMakeVisible(sourcesColumn);
    addAndMakeVisible(filterColumn);
    addAndMakeVisible(modifiersColumn);
    addAndMakeVisible(masterColumn);

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

    // 12 Slice Design: 4 columns of the synth spaced across the 12 column slices.
    const int bodyW       = bounds.getWidth();
    const int columnsSize = bodyW - (dividerThick * 3);
    const int sliceW      = columnsSize / 12;

    sourcesColumn  .setBounds   (bounds.removeFromLeft(sliceW * 4));
    dividers       .emplace_back(bounds.removeFromLeft(dividerThick).reduced(0, dividerInset));
    filterColumn   .setBounds   (bounds.removeFromLeft(sliceW * 3));
    dividers       .emplace_back(bounds.removeFromLeft(dividerThick).reduced(0, dividerInset));
    modifiersColumn.setBounds   (bounds.removeFromLeft(sliceW * 2));
    dividers       .emplace_back(bounds.removeFromLeft(dividerThick).reduced(0, dividerInset));
    masterColumn   .setBounds   (bounds.removeFromLeft(sliceW * 3));
}

void DirtyLittleBassSynthAudioProcessorEditor::timerCallback()
{
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
