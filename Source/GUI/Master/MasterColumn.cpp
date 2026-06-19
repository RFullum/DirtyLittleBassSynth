/*
  ==============================================================================

    MasterColumn.cpp

  ==============================================================================
*/

#include "MasterColumn.h"
#include "GuiHelpers.h"
#include "GUI/GuiHelpers.h"
#include "GUI/Format.h"

//==============================================================================

MasterColumn::MasterColumn(GuiResources &res)
: resources(res)
{
    setOpaque(false);
    
    auto accent = res.theme.primaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    GuiHelpers::SetupSectionLabel(this, sectionLabel, "Master", res.theme.textSecondary);

    GuiHelpers::SetupSlider(this
                            , masterGainSlider
                            , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                            , accent
                            , thumb
                            , txt);
    GuiHelpers::SetupLabel(this, masterGainLabel, "Out Gain", txt, 14.0f);

    masterGainSlider.setLookAndFeel(res.dialLookAndFeel);

    gainAtt = DLBS::AttachSlider(*res.apvts, "master_gain", masterGainSlider);

    Format::SetSliderTextFormat(masterGainSlider, Format::GainDb);

    ceilingOnButton.setClickingTogglesState(true);
    ceilingOnButton.setColour              (juce::TextButton::buttonColourId,   res.theme.structure);
    ceilingOnButton.setColour              (juce::TextButton::buttonOnColourId, res.theme.pinkAccent.withAlpha(0.25f));
    ceilingOnButton.setColour              (juce::TextButton::textColourOnId,   res.theme.pinkAccent);
    ceilingOnButton.setColour              (juce::TextButton::textColourOffId,  res.theme.textSecondary);
    addAndMakeVisible(ceilingOnButton);

    ceilingOnAtt = DLBS::AttachButton(*res.apvts, "limiter_on", ceilingOnButton);

    GuiHelpers::SetupSlider(this
                            , ceilingSlider
                            , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                            , res.theme.pinkAccent
                            , thumb
                            , txt);
    GuiHelpers::SetupLabel(this, ceilingLabel, "Ceiling", txt, 14.0f);

    ceilingSlider.setLookAndFeel    (res.dialLookAndFeel);
    ceilingSlider.setTextValueSuffix(" dB");

    ceilingAtt = DLBS::AttachSlider(*res.apvts, "limiter_ceiling", ceilingSlider);

    res.apvts->addParameterListener("limiter_on", this);

    GuiHelpers::SetupSlider(this
                            , wideSlider
                            , juce::Slider::SliderStyle::LinearHorizontal
                            , accent
                            , thumb
                            , txt);
    GuiHelpers::SetupLabel(this, wideLabel, "Wide", txt, 14.0f);

    wideSlider.setLookAndFeel           (res.dialLookAndFeel);
    wideSlider.setTextBoxStyle          (juce::Slider::NoTextBox, false, 0, 0);
    wideSlider.setDoubleClickReturnValue(true, 0.0);

    // Read by OtherLookAndFeel::drawLinearSlider: fill from track centre, not left edge.
    wideSlider.getProperties().set("bipolarFill", true);

    wideAtt = DLBS::AttachSlider(*res.apvts, "master_wide", wideSlider);

    GuiHelpers::SetupSlider(this
                            , monoCrossoverSlider
                            , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                            , res.theme.orangeAccent
                            , thumb
                            , txt);
    GuiHelpers::SetupLabel(this, monoCrossoverLabel, "Mono Below", txt, 14.0f);

    monoCrossoverSlider.setLookAndFeel    (res.dialLookAndFeel);
    monoCrossoverSlider.setTextValueSuffix(" Hz");

    monoCrossoverAtt = DLBS::AttachSlider(*res.apvts, "mono_below_freq", monoCrossoverSlider);

    outMeter.setColors(accent, res.theme.pinkAccent);
    addAndMakeVisible(outMeter);

    if (res.scopeBuffer != nullptr)
    {
        scopeVisual = std::make_unique<ScopeVisual>(*res.scopeBuffer);
        scopeVisual->SetColors(accent
                               , res.theme.background
                               , res.theme.background.darker());
        addAndMakeVisible(*scopeVisual);
    }

    ceilingOnButton.setButtonText(ceilingOnButton.getToggleState() ? "ON" : "OFF");
    RefreshCeilingEnabledLook();

    GuiHelpers::SetTip(masterGainSlider,    "Master Output Gain");
    GuiHelpers::SetTip(ceilingOnButton,     "Ceiling Limiter On/Off");
    GuiHelpers::SetTip(ceilingSlider,       "Ceiling Limiter Threshold");
    GuiHelpers::SetTip(wideSlider,          "Haas widener\nSlider position delays opposite stereo chanel");
    GuiHelpers::SetTip(monoCrossoverSlider, "Mono frequencies below crossover frequency");
}

MasterColumn::~MasterColumn()
{
    resources.apvts->removeParameterListener("limiter_on", this);
}

void MasterColumn::paint(juce::Graphics &g)
{
    // GR meter fills top-down. Display range 0..maxGRDb dB.
    constexpr float maxGRDb = 12.0f;

    g.setColour(resources.theme.structure);
    g.fillRoundedRectangle(grMeterRect.toFloat(), 1.5f);

    if (gainReductionDb > 0.001f)
    {
        const float norm  = juce::jlimit(0.0f, 1.0f, gainReductionDb / maxGRDb);
        const int   fillH = (int)(grMeterRect.getHeight() * norm);

        const auto fillRect = juce::Rectangle<int>(grMeterRect.getX(),
                                                   grMeterRect.getY(),
                                                   grMeterRect.getWidth(),
                                                   fillH);

        g.setColour(resources.theme.pinkAccent);
        g.fillRoundedRectangle(fillRect.toFloat().reduced(1.0f), 1.0f);
    }

    g.setColour(resources.theme.textSecondary.withAlpha(0.4f));
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 7.0f, 0)).withExtraKerningFactor(0.10f));
    auto grLabelArea = grMeterRect.withHeight(10).withY(grMeterRect.getBottom() - 10);
    g.drawText("GR", grLabelArea, juce::Justification::centred);
}

void MasterColumn::resized()
{
    constexpr int sectionSpacerSize    = 2;
    constexpr int sectionLabelHeight   = 16;
    constexpr int knobLabelHeight      = 16;
    constexpr int knobRowHeight        = 90;
    constexpr int meterHeight          = 90;
    constexpr int grMeterWidth         = 12;
    constexpr int grMeterGap           = 4;
    constexpr int wideRowHeight        = 50;
    constexpr int wideLabelHeight      = 16;
    constexpr int monoRowHeight        = 80;
    constexpr int monoLabelHeight      = 16;
    constexpr int gap                  = 8;
    constexpr int onButtonWidth        = 36;
    constexpr int onButtonHeight       = 14;
    constexpr int onButtonGap          = 4;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(bounds.removeFromTop(sectionLabelHeight).reduced(8, 0));
    bounds.removeFromTop(gap);

    // Each top cell: label, rotary, optional toggle pinned to the bottom.
    auto knobRow     = bounds.removeFromTop(knobRowHeight);
    auto outGainCell = knobRow.removeFromLeft(knobRow.getWidth() / 2);
    auto ceilingCell = knobRow;

    masterGainLabel .setBounds(outGainCell.removeFromTop(knobLabelHeight));
    ceilingLabel    .setBounds(ceilingCell.removeFromTop(knobLabelHeight));

    auto outGainButtonRow = outGainCell.removeFromBottom(onButtonHeight);
    auto ceilingButtonRow = ceilingCell.removeFromBottom(onButtonHeight);
    juce::ignoreUnused(outGainButtonRow);
    outGainCell.removeFromBottom(onButtonGap);
    ceilingCell.removeFromBottom(onButtonGap);

    masterGainSlider.setBounds(outGainCell);
    ceilingSlider   .setBounds(ceilingCell);

    ceilingOnButton.setBounds(ceilingButtonRow.withSizeKeepingCentre(onButtonWidth, onButtonHeight));

    bounds.removeFromTop(gap);

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

    if (scopeVisual != nullptr)
        scopeVisual->setBounds(bounds.reduced(2, 0));
}

void MasterColumn::Update(float leftLevel, float rightLevel, float gainReductionDbIn, float sampleRate)
{
    outMeter.outMeterLevel(leftLevel, rightLevel, sampleRate);

    if (! juce::approximatelyEqual(gainReductionDb, gainReductionDbIn))
    {
        gainReductionDb = gainReductionDbIn;
        repaint(grMeterRect);
    }

    if (scopeVisual != nullptr)
        scopeVisual->Update();
}

void MasterColumn::RefreshCeilingEnabledLook()
{
    const bool  on = ceilingOnButton.getToggleState();
    const float a  = on ? 1.0f : 0.4f;

    ceilingSlider.setEnabled(on);
    ceilingSlider.setAlpha  (a);
    ceilingLabel .setAlpha  (a);
}

void MasterColumn::parameterChanged(const juce::String &parameterID, float newValue)
{
    juce::ignoreUnused(newValue);

    if (parameterID == "limiter_on")
    {
        // Listener may fire on the audio thread; bounce to the message thread for UI.
        juce::Component::SafePointer<MasterColumn> self(this);
        juce::MessageManager::callAsync([self]()
        {
            if (self == nullptr)
                return;

            self->ceilingOnButton.setButtonText(self->ceilingOnButton.getToggleState() ? "ON" : "OFF");
            self->RefreshCeilingEnabledLook();
        });
    }
}
