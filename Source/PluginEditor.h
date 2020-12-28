/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OtherLookAndFeel.h"

//==============================================================================
/**
*/
class Wavetable5AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Wavetable5AudioProcessorEditor (Wavetable5AudioProcessor&);
    ~Wavetable5AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    OtherLookAndFeel otherLookAndFeel;
    
    Slider testSlider1;
    Slider testSlider2;
    Slider testSlider3;
    
    // Osc Section
    Slider oscMorphSlider;
    Slider subMorphSlider;
    Slider subGainSlider;
    ComboBox subOctave;
    
    // Osc ADSR Section
    Rectangle<float> oscADSRArea;
    
    // Modifiers Section
    Rectangle<float> modsArea;
    
    // Filter Section
    Rectangle<float> filterArea;
    
    // Filter ADSR Section
    Rectangle<float> filterADSRArea;
    
    // Filter LFO Section
    Rectangle<float> filterLFOArea;
    
    // Main Out Section
    //Rectangle<float> mainOutArea;
    Slider masterGainSlider;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Wavetable5AudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wavetable5AudioProcessorEditor)
};
