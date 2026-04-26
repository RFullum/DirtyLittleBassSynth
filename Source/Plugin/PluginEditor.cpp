#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

namespace
{
    GuiResources MakeResources(DirtyLittleBassSynthAudioProcessor &processor
                               , juce::LookAndFeel               &dialLAF
                               , juce::LookAndFeel               &dryWetLAF)
    {
        GuiResources r;
        r.apvts             = &processor.parameters;
        r.dialLookAndFeel   = &dialLAF;
        r.dryWetLookAndFeel = &dryWetLAF;
        r.onyx              = juce::Colour((juce::uint8)53,  (juce::uint8)59,  (juce::uint8)60);
        r.lightSlateGray    = juce::Colour((juce::uint8)130, (juce::uint8)146, (juce::uint8)152);
        r.magicMint         = juce::Colour((juce::uint8)174, (juce::uint8)255, (juce::uint8)216);
        r.fieryRose         = juce::Colour((juce::uint8)255, (juce::uint8)104, (juce::uint8)114);
        r.orangePeel        = juce::Colour((juce::uint8)252, (juce::uint8)152, (juce::uint8)0);
        r.textColor         = juce::Colour((juce::uint8)255, (juce::uint8)255, (juce::uint8)255);
        return r;
    }
}

//==============================================================================

DirtyLittleBassSynthAudioProcessorEditor::DirtyLittleBassSynthAudioProcessorEditor(DirtyLittleBassSynthAudioProcessor &p)
: juce::AudioProcessorEditor(&p)
, processor(p)
, resources(MakeResources(p, dialLookAndFeel, dryWetLookAndFeel))
, oscPanel       (resources)
, ampAdsrPanel   (resources)
, modifierPanel  (resources)
, filterPanel    (resources)
, filterAdsrPanel(resources)
, lfoPanel       (resources)
, masterPanel    (resources)
{
    setSize(1300, 700);

    dialLookAndFeel  .setColors(resources.fieryRose,  resources.onyx, resources.magicMint);
    dryWetLookAndFeel.setColors(resources.orangePeel, resources.onyx, resources.magicMint);

    titleHeader.setColors(resources.magicMint, resources.onyx, resources.orangePeel);
    titleFooter.setColors(resources.magicMint, resources.onyx, resources.orangePeel);

    addAndMakeVisible(titleHeader);
    addAndMakeVisible(titleFooter);
    addAndMakeVisible(oscPanel);
    addAndMakeVisible(ampAdsrPanel);
    addAndMakeVisible(modifierPanel);
    addAndMakeVisible(filterPanel);
    addAndMakeVisible(filterAdsrPanel);
    addAndMakeVisible(lfoPanel);
    addAndMakeVisible(masterPanel);

    juce::Timer::startTimerHz(60);
}

DirtyLittleBassSynthAudioProcessorEditor::~DirtyLittleBassSynthAudioProcessorEditor()
{
    juce::Timer::stopTimer();
}

void DirtyLittleBassSynthAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(resources.onyx);
}

void DirtyLittleBassSynthAudioProcessorEditor::timerCallback()
{
    oscPanel   .Update();
    lfoPanel   .Update();
    filterPanel.Update();

    auto &lvl     = processor.outputLevelBuffer;
    float magnitude = lvl.getMagnitude(0, lvl.getNumSamples());
    float outLevel  = magnitude < 0.001f ? 0.0f : magnitude;

    masterPanel.Update(outLevel, (float) processor.getSampleRate());
}

void DirtyLittleBassSynthAudioProcessorEditor::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int headerHeight      = 66;
    constexpr int footerHeight      = 17;
    constexpr int mainOutWidth      = 100;

    auto totalArea = getLocalBounds();

    titleHeader.setBounds(totalArea.removeFromTop   (headerHeight).reduced(sectionSpacerSize));
    titleFooter.setBounds(totalArea.removeFromBottom(footerHeight));

    masterPanel.setBounds(totalArea.removeFromRight(mainOutWidth).reduced(sectionSpacerSize));

    int topRowHeight = (int)(getHeight() * 0.66f);
    auto topRow      = totalArea.removeFromTop(topRowHeight);

    int oscWidth     = (int)(topRow.getWidth() * 0.40f);
    int adsrWidth    = (int)(topRow.getWidth() * 0.33f);

    oscPanel     .setBounds(topRow.removeFromLeft(oscWidth) .reduced(sectionSpacerSize));
    ampAdsrPanel .setBounds(topRow.removeFromLeft(adsrWidth).reduced(sectionSpacerSize));
    modifierPanel.setBounds(topRow                          .reduced(sectionSpacerSize));

    int bottomColumnWidth = (int)(totalArea.getWidth() * 0.33f);

    filterPanel    .setBounds(totalArea.removeFromLeft(bottomColumnWidth).reduced(sectionSpacerSize));
    filterAdsrPanel.setBounds(totalArea.removeFromLeft(bottomColumnWidth).reduced(sectionSpacerSize));
    lfoPanel       .setBounds(totalArea                                   .reduced(sectionSpacerSize));
}
