#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

namespace
{
    GuiResources MakeResources(DirtyLittleBassSynthAudioProcessor &processor
                               , juce::LookAndFeel               &dialLAF
                               , juce::LookAndFeel               &dryWetLAF)
    {
        return GuiResources{
            .apvts             = &processor.parameters,
            .dialLookAndFeel   = &dialLAF,
            .dryWetLookAndFeel = &dryWetLAF,
            .theme             = Palette::DefaultTheme,
        };
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

    dialLookAndFeel  .setColors(resources.theme.primaryAccent, resources.theme.background, resources.theme.textPrimary);
    dryWetLookAndFeel.setColors(resources.theme.orangeAccent,  resources.theme.background, resources.theme.textPrimary);

    titleHeader.setColors(resources.theme.primaryAccent, resources.theme.background, resources.theme.textPrimary);
    titleFooter.setColors(resources.theme.primaryAccent, resources.theme.background, resources.theme.textPrimary);

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
    g.fillAll(resources.theme.background);
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
