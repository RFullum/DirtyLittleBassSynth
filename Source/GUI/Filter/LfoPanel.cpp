/*
  ==============================================================================

    LfoPanel.cpp

  ==============================================================================
*/

#include "LfoPanel.h"
#include "GuiHelpers.h"

//============================================================

LfoPanel::LfoPanel(GuiResources &res)
: resources(res)
{
    setOpaque(false);
    
    auto accent = res.theme.secondaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    DLBS::SetupSlider(this, lfoShapeSlider,   juce::Slider::SliderStyle::LinearHorizontal, accent, thumb, txt);
    DLBS::SetupSlider(this, lfoFreqSlider,    juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    DLBS::SetupSlider(this, lfoSyncDivSlider, juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);
    DLBS::SetupSlider(this, lfoAmountSlider,  juce::Slider::SliderStyle::LinearVertical,   accent, thumb, txt);

    // LFO shape slider sits directly under the LFO visual; suppress its value textbox
    // so the slider track spans the full width of the visual above it.
    lfoShapeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    lfoShapeSlider  .setLookAndFeel(res.dialLookAndFeel);
    lfoFreqSlider   .setLookAndFeel(res.dialLookAndFeel);
    lfoSyncDivSlider.setLookAndFeel(res.dialLookAndFeel);
    lfoAmountSlider .setLookAndFeel(res.dialLookAndFeel);

    DLBS::SetupSectionLabel(this, sectionLabel, "Filter LFO", res.theme.textSecondary);

    DLBS::SetupLabel(this, lfoRateLabel,   "Freq",   txt, 14.0f);
    DLBS::SetupLabel(this, lfoAmountLabel, "Amount", txt, 14.0f);

    shapeAtt  = DLBS::AttachSlider(*res.apvts, "filtLFO_shape",    lfoShapeSlider);
    freqAtt   = DLBS::AttachSlider(*res.apvts, "filtLFO_freq",     lfoFreqSlider);
    divAtt    = DLBS::AttachSlider(*res.apvts, "filtLFO_sync_div", lfoSyncDivSlider);
    amountAtt = DLBS::AttachSlider(*res.apvts, "filtLFO_amt",      lfoAmountSlider);

    // Display the choice's text label ("1/8", "1/4D", etc.) in the slider's
    // textbox by overriding textFromValueFunction. The slider attachment maps
    // 0..count-1 integer values onto the choice indices.
    if (auto *divParam = dynamic_cast<juce::AudioParameterChoice *>(res.apvts->getParameter("filtLFO_sync_div")))
    {
        const auto &choices = divParam->choices;

        lfoSyncDivSlider.textFromValueFunction = [&choices](double value) -> juce::String
        {
            const int idx = juce::jlimit(0, choices.size() - 1, (int) std::round(value));
            return choices[idx];
        };

        // Force the textbox to refresh now that the lambda is in place.
        lfoSyncDivSlider.updateText();
    }

    // FRQ / SYNC toggle, attached to filtLFO_sync (bool param → 2 segments).
    lfoSyncControl.Setup(*res.apvts, "filtLFO_sync",
                         juce::StringArray({"FRQ", "SYNC"}),
                         accent, res.theme.structure, res.theme.textSecondary);
    addAndMakeVisible(lfoSyncControl);

    auto bg     = res.theme.background;
    auto bgFade = res.theme.background.darker();
    lfoVisual.SetColors(accent, bg, bgFade);
    lfoVisual.Init(res.apvts->getRawParameterValue("filtLFO_shape"), /*useSquare*/ true);

    addAndMakeVisible(lfoVisual);

    // Listen for FRQ/SYNC changes from any source (UI click, automation, preset
    // recall). The listener flips slider visibility + label text.
    res.apvts->addParameterListener("filtLFO_sync", this);

    RefreshSyncModeLook();
}

LfoPanel::~LfoPanel()
{
    resources.apvts->removeParameterListener("filtLFO_sync", this);
}

void LfoPanel::paint(juce::Graphics &)
{
    // Sub-components draw themselves; nothing custom for LfoPanel itself.
}

void LfoPanel::resized()
{
    static constexpr int sectionSpacerSize = 2;
    static constexpr int sliderLabelH      = 13;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);
    sectionLabel.setBounds(bounds.removeFromTop(16));
    bounds.removeFromBottom(sectionSpacerSize);

    // Right cluster: rate (Hz / sync) and amount vertical sliders side by side.
    auto slidersArea = bounds.removeFromRight(100);
    auto amtArea     = slidersArea.removeFromRight(slidersArea.proportionOfWidth(0.5f));

    lfoAmountLabel  .setBounds(amtArea.removeFromTop(sliderLabelH));
    lfoAmountSlider .setBounds(amtArea);

    lfoRateLabel    .setBounds(slidersArea.removeFromTop(sliderLabelH));
    // Both rate sliders share the same area; visibility is toggled by the sync mode.
    lfoFreqSlider   .setBounds(slidersArea);
    lfoSyncDivSlider.setBounds(slidersArea);

    // Bottom strip: FRQ / SYNC toggle on the left, room left for future use.
    auto syncRow = bounds.removeFromBottom(25).reduced(sectionSpacerSize * 2, sectionSpacerSize);
    syncRowRect  = syncRow;

    const int toggleWidth = juce::jmin(syncRow.getWidth() / 2, 110);
    lfoSyncControl.setBounds(syncRow.removeFromLeft(toggleWidth));

    lfoShapeSlider.setBounds(bounds.removeFromBottom(30));
    lfoVisual     .setBounds(bounds.reduced(sectionSpacerSize));
}

void LfoPanel::Update()
{
    lfoVisual.Update();
}

void LfoPanel::parameterChanged(const juce::String &parameterID, float newValue)
{
    juce::ignoreUnused(newValue);

    if (parameterID == "filtLFO_sync")
    {
        // Listener may fire on the audio thread; bounce to the message thread for UI.
        juce::Component::SafePointer<LfoPanel> self(this);
        juce::MessageManager::callAsync([self]()
        {
            if (self != nullptr)
                self->RefreshSyncModeLook();
        });
    }
}

void LfoPanel::RefreshSyncModeLook()
{
    const bool sync = lfoSyncControl.GetSelectedIndex() == 1;

    lfoFreqSlider   .setVisible(! sync);
    lfoSyncDivSlider.setVisible(  sync);

    lfoRateLabel.setText(sync ? "Sync" : "Freq", juce::dontSendNotification);
}
