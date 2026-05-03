/*
  ==============================================================================

    MasterColumn.cpp

  ==============================================================================
*/

#include "MasterColumn.h"
#include "GuiHelpers.h"

//============================================================

MasterColumn::MasterColumn(GuiResources &res)
: resources(res)
{
    auto accent = res.theme.primaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    DLBS::SetupSectionLabel(this, sectionLabel, "Master", res.theme.textSecondary);

    DLBS::SetupSlider(this
                      , masterGainSlider
                      , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                      , accent
                      , thumb
                      , txt);
    DLBS::SetupLabel(this, masterGainLabel, "Out Gain", txt, 14.0f);

    masterGainSlider.setLookAndFeel(res.dialLookAndFeel);

    gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts
                                                                                     , "master_gain"
                                                                                     , masterGainSlider);

    DLBS::SetSliderTextFormat(masterGainSlider, DLBS::FormatGainDb);

    // === Limiter Ceiling (placeholder, no APVTS / DSP yet) ===
    ceilingOnButton.setButtonText          ("ON");
    ceilingOnButton.setClickingTogglesState(true);
    ceilingOnButton.setToggleState         (true, juce::dontSendNotification);
    ceilingOnButton.setColour              (juce::TextButton::buttonColourId,   res.theme.structure);
    ceilingOnButton.setColour              (juce::TextButton::buttonOnColourId, res.theme.pinkAccent.withAlpha(0.25f));
    ceilingOnButton.setColour              (juce::TextButton::textColourOnId,   res.theme.pinkAccent);
    ceilingOnButton.setColour              (juce::TextButton::textColourOffId,  res.theme.textSecondary);
    ceilingOnButton.onClick = [this]()
    {
        ceilingOnButton.setButtonText(ceilingOnButton.getToggleState() ? "ON" : "OFF");
        RefreshCeilingEnabledLook();
    };
    addAndMakeVisible(ceilingOnButton);

    DLBS::SetupSlider(this
                      , ceilingSlider
                      , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                      , res.theme.pinkAccent
                      , thumb
                      , txt);
    DLBS::SetupLabel(this, ceilingLabel, "Ceiling", txt, 14.0f);

    ceilingSlider.setLookAndFeel    (res.dialLookAndFeel);
    ceilingSlider.setRange          (-12.0, 0.0, 0.1);
    ceilingSlider.setValue          (-0.1, juce::dontSendNotification);
    ceilingSlider.setTextValueSuffix(" dB");

    // === Haas Widener (placeholder). Bipolar slider; center = mono. ===
    DLBS::SetupSlider(this
                      , wideSlider
                      , juce::Slider::SliderStyle::LinearHorizontal
                      , accent
                      , thumb
                      , txt);
    DLBS::SetupLabel(this, wideLabel, "Wide", txt, 14.0f);

    wideSlider.setLookAndFeel           (res.dialLookAndFeel);
    wideSlider.setRange                 (-1.0, 1.0, 0.001);
    wideSlider.setValue                 (0.0, juce::dontSendNotification);
    wideSlider.setTextBoxStyle          (juce::Slider::NoTextBox, false, 0, 0);
    wideSlider.setDoubleClickReturnValue(true, 0.0);

    // Tells OtherLookAndFeel::drawLinearSlider to fill from the centre of the track
    // to the thumb instead of from the left edge.
    wideSlider.getProperties().set("bipolarFill", true);

    // === Bass Mono-izer crossover (placeholder) ===
    DLBS::SetupSlider(this
                      , monoCrossoverSlider
                      , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                      , res.theme.orangeAccent
                      , thumb
                      , txt);
    DLBS::SetupLabel(this, monoCrossoverLabel, "Mono Below", txt, 14.0f);

    monoCrossoverSlider.setLookAndFeel    (res.dialLookAndFeel);
    monoCrossoverSlider.setRange          (20.0, 300.0, 1.0);
    monoCrossoverSlider.setValue          (120.0, juce::dontSendNotification);
    monoCrossoverSlider.setTextValueSuffix(" Hz");

    outMeter.setColors(accent, res.theme.pinkAccent);
    addAndMakeVisible(outMeter);

    RefreshCeilingEnabledLook();
}

void MasterColumn::paint(juce::Graphics &g)
{
    // Scope placeholder — outlined rect with "SCOPE" label.
    g.setColour(resources.theme.structure);
    g.drawRoundedRectangle(scopeRect.toFloat().reduced(0.5f), 2.0f, 1.0f);

    g.setColour(resources.theme.textSecondary.withAlpha(0.4f));
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 8.0f, 0))
                 .withExtraKerningFactor(0.12f));
    g.drawText("SCOPE", scopeRect, juce::Justification::centred);

    // GR meter placeholder — vertical outlined bar next to the output meter.
    g.setColour(resources.theme.structure);
    g.drawRoundedRectangle(grMeterRect.toFloat().reduced(0.5f), 1.5f, 1.0f);

    g.setColour(resources.theme.textSecondary.withAlpha(0.4f));
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 7.0f, 0)).withExtraKerningFactor(0.10f));
    auto grLabelArea = grMeterRect.withHeight(10).withY(grMeterRect.getBottom() - 10);
    g.drawText("GR", grLabelArea, juce::Justification::centred);
}

void MasterColumn::resized()
{
    constexpr int sectionSpacerSize    = 2;
    constexpr int sectionLabelHeight   = 16;
    constexpr int knobLabelHeight      = 18;
    constexpr int knobRowHeight        = 90;
    constexpr int meterHeight          = 90;
    constexpr int grMeterWidth         = 12;
    constexpr int grMeterGap           = 4;
    constexpr int wideRowHeight        = 50;
    constexpr int wideLabelHeight      = 16;
    constexpr int monoRowHeight        = 80;
    constexpr int monoLabelHeight      = 16;
    constexpr int gap                  = 8;
    constexpr int onButtonWidth        = 28;
    constexpr int onButtonHeight       = 14;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(bounds.removeFromTop(sectionLabelHeight).reduced(8, 0));
    bounds.removeFromTop(gap);

    // === Top: Out Gain rotary | Ceiling (toggle + rotary), side by side ===
    auto knobRow      = bounds.removeFromTop(knobRowHeight);
    auto outGainCell  = knobRow.removeFromLeft(knobRow.getWidth() / 2);
    auto ceilingCell  = knobRow;

    auto outGainLabel = outGainCell.removeFromTop(knobLabelHeight);
    masterGainLabel .setBounds(outGainLabel);
    masterGainSlider.setBounds(outGainCell);

    // Ceiling label strip: ON button (left) + "Ceiling" label (rest).
    auto ceilLabelStrip = ceilingCell.removeFromTop(knobLabelHeight);
    auto ceilOnArea     = ceilLabelStrip.removeFromLeft(onButtonWidth)
                                        .withSizeKeepingCentre(onButtonWidth, onButtonHeight);
    ceilingOnButton.setBounds(ceilOnArea);
    ceilingLabel   .setBounds(ceilLabelStrip);
    ceilingSlider  .setBounds(ceilingCell);

    bounds.removeFromTop(gap);

    // === Bottom-up: mono-izer, widener, output meter (with GR meter to the right) ===
    auto monoRow       = bounds.removeFromBottom(monoRowHeight);
    auto monoLabelArea = monoRow.removeFromTop(monoLabelHeight);
    monoCrossoverLabel .setBounds(monoLabelArea);
    monoCrossoverSlider.setBounds(monoRow);

    bounds.removeFromBottom(gap);

    auto wideRow       = bounds.removeFromBottom(wideRowHeight);
    auto wideLabelArea = wideRow.removeFromTop(wideLabelHeight);
    wideLabel .setBounds(wideLabelArea);
    wideSlider.setBounds(wideRow.reduced(8, 4));

    bounds.removeFromBottom(gap);

    auto meterRow = bounds.removeFromBottom(meterHeight);
    auto grArea   = meterRow.removeFromRight(grMeterWidth);
    meterRow.removeFromRight(grMeterGap);

    outMeter.setBounds(meterRow);
    grMeterRect = grArea;

    bounds.removeFromBottom(gap);

    // === Middle: scope fills the remaining space (square-ish in a typical column) ===
    scopeRect = bounds.reduced(2, 0);
}

void MasterColumn::Update(float leftLevel, float rightLevel, float sampleRate)
{
    outMeter.outMeterLevel(leftLevel, rightLevel, sampleRate);
}

void MasterColumn::RefreshCeilingEnabledLook()
{
    const float a = ceilingOnButton.getToggleState() ? 1.0f : 0.4f;
    ceilingSlider.setAlpha(a);
    ceilingLabel .setAlpha(a);
}
