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
    
    /// Sets up slider: Takes reference to a slider, the style of the slider, and the color of the fill
    void sliderSetup(Slider& sliderInstance, Slider::SliderStyle style, Colour sliderFillColor);
    
    /// Sets up Label: Takes label reference, and the text
    void sliderLabelSetup(Label& labelInstance, String labelText);
    
    Slider testSlider1;
    Slider testSlider2;
    Slider testSlider3;
    
    // Osc Section
    Slider oscMorphSlider;
    Slider subMorphSlider;
    Slider subGainSlider;
    ComboBox subOctave;
    
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
    
    Label ringLabel;
    Label frqShftLabel;
    Label sHLabel;
    Label toneLabel;
    Label pitchLabel;
    Label dryWetLabel;
    
    // Filter Section
    Slider cutoffSlider;
    Slider resSlider;
    
    Label cutoffLabel;
    Label resLabel;
    
    // Filter ADSR Section
    Slider fltAttackSlider;
    Slider fltDecaySlider;
    Slider fltSustainSlider;
    Slider fltReleaseSlider;
    Slider adsrToCutoffSlider;
    Slider adsrToResSlider;
    
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
    
    Label lfoShapeLabel;
    Label lfoFreqLabel;
    Label lfoAmountLabel;
    
    // Main Out Section
    Slider masterGainSlider;
    
    Label masterGainLabel;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Wavetable5AudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wavetable5AudioProcessorEditor)
};
