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

    dialLookAndFeel  .setTrackBackground(resources.theme.structure);
    dryWetLookAndFeel.setTrackBackground(resources.theme.structure);

    titleHeader.setTheme(resources.theme);
    titleFooter.setTheme(resources.theme);

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

    g.setColour(resources.theme.structure);
    for (const auto &d : dividers)
        g.fillRect(d);
}

void DirtyLittleBassSynthAudioProcessorEditor::resized()
{
    constexpr int headerHeight  = 66;
    constexpr int footerHeight  = 17;
    constexpr int masterWidth   = 100;
    constexpr int dividerThick  = 1;

    const int W = getWidth();
    const int H = getHeight();

    titleHeader.setBounds(0, 0,                     W, headerHeight);
    titleFooter.setBounds(0, H - footerHeight,      W, footerHeight);

    const int bodyTop    = headerHeight;
    const int bodyBottom = H - footerHeight;
    const int bodyHeight = bodyBottom - bodyTop;

    const int masterLeft = W - masterWidth;
    masterPanel.setBounds(masterLeft, bodyTop, masterWidth, bodyHeight);

    const int mainLeft   = 0;
    const int mainRight  = masterLeft;
    const int mainWidth  = mainRight - mainLeft;

    const int topRowHeight = (int)(bodyHeight * 0.66f);
    const int rowSplitY    = bodyTop + topRowHeight;

    const int oscWidth     = (int)(mainWidth * 0.40f);
    const int adsrWidth    = (int)(mainWidth * 0.33f);
    const int oscRight     = mainLeft + oscWidth;
    const int adsrRight    = oscRight + adsrWidth;

    oscPanel     .setBounds(mainLeft,  bodyTop,    oscWidth,             topRowHeight);
    ampAdsrPanel .setBounds(oscRight,  bodyTop,    adsrWidth,            topRowHeight);
    modifierPanel.setBounds(adsrRight, bodyTop,    mainRight - adsrRight, topRowHeight);

    const int bottomCol     = (int)(mainWidth * 0.33f);
    const int filterRight   = mainLeft + bottomCol;
    const int fltAdsrRight  = filterRight + bottomCol;
    const int bottomHeight  = bodyBottom - rowSplitY;

    filterPanel    .setBounds(mainLeft,      rowSplitY, bottomCol,                  bottomHeight);
    filterAdsrPanel.setBounds(filterRight,   rowSplitY, bottomCol,                  bottomHeight);
    lfoPanel       .setBounds(fltAdsrRight,  rowSplitY, mainRight - fltAdsrRight,   bottomHeight);

    // Section dividers: thin lines between adjacent panels. Verticals are inset
    // from horizontal dividers so the lines never meet at a vertex — keeps each
    // intersection a clean break instead of a + or T.
    constexpr int dividerInset = 8;

    dividers.clear();

    // Horizontal: under header (full width); above footer (full width).
    dividers.emplace_back(0, headerHeight,            W, dividerThick);
    dividers.emplace_back(0, bodyBottom,              W, dividerThick);

    // Horizontal: between top row and bottom row, only across the main area.
    dividers.emplace_back(0, rowSplitY,               masterLeft, dividerThick);

    // Vertical: between main area and master column. Inset from the header /
    // footer / row-split horizontals.
    dividers.emplace_back(masterLeft - dividerThick,  bodyTop + dividerInset,
                          dividerThick, bodyHeight - dividerInset * 2);

    // Vertical: between top-row panels. Inset from header divider above and
    // row-split divider below.
    dividers.emplace_back(oscRight  - dividerThick,   bodyTop + dividerInset,
                          dividerThick, topRowHeight - dividerInset * 2);
    dividers.emplace_back(adsrRight - dividerThick,   bodyTop + dividerInset,
                          dividerThick, topRowHeight - dividerInset * 2);

    // Vertical: between bottom-row panels. Inset from row-split above and
    // footer divider below.
    dividers.emplace_back(filterRight  - dividerThick, rowSplitY + dividerInset,
                          dividerThick, bottomHeight - dividerInset * 2);
    dividers.emplace_back(fltAdsrRight - dividerThick, rowSplitY + dividerInset,
                          dividerThick, bottomHeight - dividerInset * 2);
}

void DirtyLittleBassSynthAudioProcessorEditor::timerCallback()
{
    oscPanel   .Update();
    lfoPanel   .Update();
    filterPanel.Update();

    auto      &lvl       = processor.outputLevelBuffer;
    const int  n         = lvl.getNumSamples();
    const int  numChans  = lvl.getNumChannels();

    float leftMag  = (numChans > 0 && n > 0) ? lvl.getMagnitude(0, 0, n) : 0.0f;
    float rightMag = (numChans > 1 && n > 0) ? lvl.getMagnitude(1, 0, n) : leftMag;

    if (leftMag  < 0.001f) leftMag  = 0.0f;
    if (rightMag < 0.001f) rightMag = 0.0f;

    masterPanel.Update(leftMag, rightMag, (float) processor.getSampleRate());
}
