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
#include "OscVisual.h"

//==============================================================================
/**
*/
class DirtyLittleBassSynthAudioProcessorEditor  : public AudioProcessorEditor, public Timer
{
public:
    DirtyLittleBassSynthAudioProcessorEditor (DirtyLittleBassSynthAudioProcessor&);
    ~DirtyLittleBassSynthAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    OtherLookAndFeel otherLookAndFeel;
    
    /// Sets up slider: Takes reference to a slider, the style of the slider, and the color of the fill
    void sliderSetup(Slider& sliderInstance, Slider::SliderStyle style, Colour sliderFillColor);
    
    /// Sets up Label: Takes label reference, and the text
    void sliderLabelSetup(Label& labelInstance, String labelText);
    
    /// ComboBox setup
    void comboBoxSetup(ComboBox& boxInstance, StringArray boxItems);
    
    // Wavetable Drawing
    OscVisual oscVisual;
    OscVisual subOscVisual;
    OscVisual lfoVisual;
    
    Rectangle<int> oscVisualSpace;
    
    //Slider testSlider1;
    //Slider testSlider2;
    //Slider testSlider3;
    
    // Osc Section
    Slider oscMorphSlider;
    Slider subMorphSlider;
    Slider subGainSlider;
    ComboBox subOctave;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscMorphSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> subMorphSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> subGainSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> subOctaveAttachment;
    // WHY ISN'T THE COMBO BOX SHOWING ITS MEMBERS?
    
    Label oscMorphLabel;
    Label subMorphLabel;
    Label subGainLabel;
    
    // Osc ADSR Section
    Slider oscAttackSlider;
    Slider oscDecaySlider;
    Slider oscSustainSlider;
    Slider oscReleaseSlider;
    Slider portaSlider;
    Slider foldbackSlider;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscAttackSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscDecaySliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscSustainSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> oscReleaseSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> portaSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> foldbackSliderAttachment;
    
    Label oscAttackLabel;
    Label oscDecayLabel;
    Label oscSustainLabel;
    Label oscReleaseLabel;
    Label portaLabel;
    Label foldbackLabel;
    
    // Modifiers Section
    Slider ringToneSlider;
    Slider ringPitchSlider;
    Slider ringDryWetSlider;
    Slider frqShftPitchSlider;
    Slider frqShftDryWetSlider;
    Slider sHPitchSlider;
    Slider sHDryWetSlider;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ringToneSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ringPitchSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ringDryWetSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> frqShftPitchSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> frqShftDryWetSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sHPitchSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sHDryWetSliderAttachment;
    
    Label ringLabel;
    Label frqShftLabel;
    Label sHLabel;
    Label toneLabel;
    Label pitchLabel;
    Label dryWetLabel;
    
    // Filter Section
    Slider cutoffSlider;
    Slider resSlider;
    ComboBox filterType;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> cutoffSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> resSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    
    Label cutoffLabel;
    Label resLabel;
    
    // Filter ADSR Section
    Slider fltAttackSlider;
    Slider fltDecaySlider;
    Slider fltSustainSlider;
    Slider fltReleaseSlider;
    Slider adsrToCutoffSlider;
    Slider adsrToResSlider;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> fltAttackSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> fltDecaySliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> fltSustainSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> fltReleaseSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> adsrToCutoffSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> adsrToResSliderAttachment;
    
    Label fltAttackLabel;
    Label fltDecayLabel;
    Label fltSustainLabel;
    Label fltReleaseLabel;
    Label adsrToCutoffLabel;
    Label adsrToResLabel;
    
    
    // Filter LFO Section
    Slider lfoShapeSlider;
    Slider lfoFreqSlider;
    Slider lfoAmountSlider;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoShapeSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoFreqSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lfoAmountSliderAttachment;
    
    Label lfoShapeLabel;
    Label lfoFreqLabel;
    Label lfoAmountLabel;
    
    // Main Out Section
    Slider masterGainSlider;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> masterGainSliderAttachment;
    
    Label masterGainLabel;
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DirtyLittleBassSynthAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirtyLittleBassSynthAudioProcessorEditor)
};
