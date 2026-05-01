/*
  ==============================================================================

    SegmentedControl.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//============================================================

/// Horizontal segmented button row driving a juce::AudioParameterChoice. Replaces
/// a ComboBox where a fixed, small number of options (2-5) reads better as inline
/// pills. Owns a ParameterAttachment so changes flow both ways with the APVTS.
class SegmentedControl
    : public juce::Component
{
public:
    SegmentedControl();
    ~SegmentedControl() override = default;

    /// Builds N segments labelled by the strings in items, attaches to the named
    /// choice parameter, and styles segments with the given colors.
    /// activeFill is used for the selected segment's background; outline / inactive
    /// text use outlineColor; text uses textColor.
    void Setup(juce::AudioProcessorValueTreeState &apvts
               , const juce::String              &parameterID
               , const juce::StringArray         &items
               , juce::Colour                     activeFill
               , juce::Colour                     outlineColor
               , juce::Colour                     textColor
               , float                            fontSize = 10.0f);

    /// 0-based index of the currently selected segment.
    int GetSelectedIndex() const noexcept { return selectedIndex; }

    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &) override;

private:
    void SyncFromParameter();

    juce::StringArray                 segments;
    std::vector<juce::Rectangle<int>> segmentBounds;
    int                               selectedIndex = 0;

    juce::Colour activeFill;
    juce::Colour outlineColor;
    juce::Colour textColor;
    float        fontSize = 10.0f;

    juce::RangedAudioParameter        *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SegmentedControl)
};
