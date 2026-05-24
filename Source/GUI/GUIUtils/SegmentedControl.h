/*
  ==============================================================================

    SegmentedControl.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

// Horizontal pill-row bound to a juce::AudioParameterChoice. Suits small
// fixed option sets (2-5) better than a ComboBox.
class SegmentedControl
    : public juce::Component
    , public juce::SettableTooltipClient
{
public:
    SegmentedControl();
    ~SegmentedControl() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &) override;

    void Setup(juce::AudioProcessorValueTreeState &apvts
               , const juce::String               &parameterID
               , const juce::StringArray          &items
               , juce::Colour                      activeFill
               , juce::Colour                      outlineColor
               , juce::Colour                      textColor
               , float                             fontSize = 12.0f);

    int GetSelectedIndex() const noexcept { return selectedIndex; }

private:
    void SyncFromParameter();

    juce::StringArray                 segments;
    std::vector<juce::Rectangle<int>> segmentBounds;
    int                               selectedIndex = 0;

    juce::Colour activeFill;
    juce::Colour outlineColor;
    juce::Colour textColor;
    float        fontSize = 12.0f;

    juce::RangedAudioParameter *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SegmentedControl)
};
