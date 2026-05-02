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

    DLBS::SetupSlider(this, masterGainSlider, juce::Slider::SliderStyle::LinearVertical, accent, thumb, txt);
    DLBS::SetupLabel (this, masterGainLabel,  "Out Gain", txt, 16.0f);

    masterGainSlider.setLookAndFeel(res.dialLookAndFeel);

    gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "master_gain", masterGainSlider);

    DLBS::SetSliderTextFormat(masterGainSlider, DLBS::FormatGainDb);

    outMeter.setColors(accent, res.theme.pinkAccent);
    addAndMakeVisible(outMeter);
}

void MasterColumn::paint(juce::Graphics &g)
{
    // Decorative outlined placeholder for the future output scope.
    g.setColour(resources.theme.structure);
    g.drawRoundedRectangle(scopeRect.toFloat().reduced(0.5f), 2.0f, 1.0f);

    g.setColour(resources.theme.textSecondary.withAlpha(0.4f));
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 8.0f, 0))
                 .withExtraKerningFactor(0.12f));
    g.drawText("SCOPE", scopeRect, juce::Justification::centred);
}

void MasterColumn::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int mainLabelHeight   = 30;
    constexpr int scopeHeight       = 40;
    constexpr int scopeGap          = 6;

    auto bounds = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(bounds.removeFromTop(16).reduced(8, 0));

    auto labelArea = bounds.removeFromTop(mainLabelHeight);
    masterGainLabel.setBounds(labelArea);

    // Reserve the meter area at the very bottom and the scope just above it.
    const int meterHeight = (bounds.getHeight() - scopeHeight - scopeGap * 2) / 2;
    auto meterArea = bounds.removeFromBottom(juce::jmax(60, meterHeight));
    outMeter.setBounds(meterArea);

    bounds.removeFromBottom(scopeGap);

    auto scopeArea = bounds.removeFromBottom(scopeHeight);
    scopeRect = scopeArea.reduced(4, 0);

    bounds.removeFromBottom(scopeGap);

    masterGainSlider.setBounds(bounds);
}

void MasterColumn::Update(float leftLevel, float rightLevel, float sampleRate)
{
    outMeter.outMeterLevel(leftLevel, rightLevel, sampleRate);
}
