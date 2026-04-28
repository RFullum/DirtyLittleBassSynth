/*
  ==============================================================================

    OscPanel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiResources.h"
#include "OscVisual.h"

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
    GuiResources &resources;

    OscVisual oscVisual;
    OscVisual subOscVisual;

    juce::Slider   oscMorphSlider;
    juce::Slider   subMorphSlider;
    juce::Slider   subGainSlider;
    juce::Slider   pitchBendRangeSlider;
    juce::ComboBox subOctave;

    juce::Label sectionLabel;
    juce::Label oscMorphLabel;
    juce::Label oscMorphLabel2;
    juce::Label subMorphLabel;
    juce::Label subMorphLabel2;
    juce::Label subGainLabel;
    juce::Label pitchBendRangeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   oscMorphAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   subMorphAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   subGainAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   pitchBendRangeAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> subOctaveAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscPanel)
};
