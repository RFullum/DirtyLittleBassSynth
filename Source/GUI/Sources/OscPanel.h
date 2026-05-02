/*
  ==============================================================================

    OscPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OscVisual.h"
#include "SegmentedControl.h"

//============================================================

/// Top-left section: main oscillator (morph + bend range + visual) stacked over
/// sub oscillator (morph + sub gain + sub octave + visual).
class OscPanel
    : public juce::Component
{
public:
    OscPanel(GuiResources &res);

    void resized() override;

    /// Called from the editor's timer so the wave visuals can rebuild their paths.
    void Update();

private:
    OscVisual oscVisual;
    OscVisual subOscVisual;

    juce::Slider     oscMorphSlider;
    juce::Slider     subMorphSlider;
    juce::Slider     subGainSlider;
    juce::Slider     pitchBendRangeSlider;
    SegmentedControl subOctave;

    juce::Label sectionLabel;
    juce::Label oscLabel;
    juce::Label morphOscLabel;
    juce::Label subLabel;
    juce::Label morphSubLabel;
    juce::Label subGainLabel;
    juce::Label pitchBendRangeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   oscMorphAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   subMorphAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   subGainAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   pitchBendRangeAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscPanel)
};
