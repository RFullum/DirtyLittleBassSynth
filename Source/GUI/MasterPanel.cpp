/*
  ==============================================================================

    MasterPanel.cpp

  ==============================================================================
*/

#include "MasterPanel.h"
#include "GuiHelpers.h"

//============================================================

MasterPanel::MasterPanel(GuiResources &res)
: resources(res)
{
    using namespace dlbs;

    auto accent = res.theme.primaryAccent;
    auto thumb  = res.theme.textPrimary;
    auto txt    = res.theme.textPrimary;

    SetupSectionLabel(this, sectionLabel, "Master", res.theme.textSecondary);

    SetupSlider(this, masterGainSlider, juce::Slider::SliderStyle::LinearVertical, accent, thumb, txt);
    SetupLabel (this, masterGainLabel,  "Out Gain", txt, 16.0f);

    masterGainSlider.setLookAndFeel(res.dialLookAndFeel);

    gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*res.apvts, "master_gain", masterGainSlider);

    SetSliderTextFormat(masterGainSlider, FormatGainDb);

    outMeter.setColors(accent, res.theme.pinkAccent);
    addAndMakeVisible(outMeter);
}

void MasterPanel::paint(juce::Graphics &g)
{
    // Decorative outlined placeholder for the future output scope.
    g.setColour(resources.theme.structure);
    g.drawRoundedRectangle(scopeRect.toFloat().reduced(0.5f), 2.0f, 1.0f);

    g.setColour(resources.theme.textSecondary.withAlpha(0.4f));
    g.setFont(juce::Font(juce::FontOptions("Helvetica", 8.0f, 0))
                 .withExtraKerningFactor(0.12f));
    g.drawText("SCOPE", scopeRect, juce::Justification::centred);
}

void MasterPanel::resized()
{
    constexpr int sectionSpacerSize = 2;
    constexpr int mainLabelHeight   = 30;
    constexpr int scopeHeight       = 40;
    constexpr int scopeGap          = 6;

    auto area = getLocalBounds().reduced(sectionSpacerSize);

    sectionLabel.setBounds(area.removeFromTop(16).reduced(8, 0));

    auto labelArea = area.removeFromTop(mainLabelHeight);
    masterGainLabel.setBounds(labelArea);

    // Reserve the meter area at the very bottom and the scope just above it.
    const int meterHeight = (area.getHeight() - scopeHeight - scopeGap * 2) / 2;
    auto meterArea = area.removeFromBottom(juce::jmax(60, meterHeight));
    outMeter.setBounds(meterArea);

    area.removeFromBottom(scopeGap);

    auto scopeArea = area.removeFromBottom(scopeHeight);
    scopeRect = scopeArea.reduced(4, 0);

    area.removeFromBottom(scopeGap);

    masterGainSlider.setBounds(area);
}

void MasterPanel::Update(float leftLevel, float rightLevel, float sampleRate)
{
    outMeter.outMeterLevel(leftLevel, rightLevel, sampleRate);
}
