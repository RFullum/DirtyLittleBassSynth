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
, sourcesColumn  (resources)
, filterColumn   (resources)
, modifiersColumn(resources)
, masterColumn   (resources)
{
    setSize(1300, 700);

    dialLookAndFeel  .setTrackBackground(resources.theme.structure);
    dryWetLookAndFeel.setTrackBackground(resources.theme.structure);

    titleHeader.setTheme(resources.theme);
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
    constexpr int headerHeight = 66;
    constexpr int footerHeight = 17;
    constexpr int dividerThick = 1;
    constexpr int dividerInset = 8;

    const int W = getWidth();
    const int H = getHeight();

    titleHeader.setBounds(0, 0,                W, headerHeight);
    titleFooter.setBounds(0, H - footerHeight, W, footerHeight);

    const int bodyTop    = headerHeight;
    const int bodyBottom = H - footerHeight;
    const int bodyHeight = bodyBottom - bodyTop;

    // Four equal-width columns spanning the full body height.
    const int colCount  = 4;
    const int colWidth  = W / colCount;
    const int col0Left  = 0;
    const int col1Left  = colWidth;
    const int col2Left  = colWidth * 2;
    const int col3Left  = colWidth * 3;

    sourcesColumn  .setBounds(col0Left, bodyTop, colWidth,         bodyHeight);
    filterColumn   .setBounds(col1Left, bodyTop, colWidth,         bodyHeight);
    modifiersColumn.setBounds(col2Left, bodyTop, colWidth,         bodyHeight);
    masterColumn   .setBounds(col3Left, bodyTop, W - col3Left,     bodyHeight);

    dividers.clear();

    // Horizontal: under header (full width); above footer (full width).
    dividers.emplace_back(0, headerHeight, W, dividerThick);
    dividers.emplace_back(0, bodyBottom,   W, dividerThick);

    // Vertical dividers between columns. Inset from header / footer so the lines
    // never meet at a vertex — keeps each intersection a clean break.
    dividers.emplace_back(col1Left - dividerThick, bodyTop + dividerInset,
                          dividerThick, bodyHeight - dividerInset * 2);
    dividers.emplace_back(col2Left - dividerThick, bodyTop + dividerInset,
                          dividerThick, bodyHeight - dividerInset * 2);
    dividers.emplace_back(col3Left - dividerThick, bodyTop + dividerInset,
                          dividerThick, bodyHeight - dividerInset * 2);
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

    masterColumn.Update(leftMag, rightMag, (float) processor.getSampleRate());
}
