#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OtherLookAndFeel.h"
#include "OscVisual.h"
#include "FilterVisual.h"
#include "OutMeter.h"
#include "TitleHeader.h"

//==============================================================================

class DirtyLittleBassSynthAudioProcessorEditor
: public  juce::AudioProcessorEditor
, private juce::Timer
{
public:
    DirtyLittleBassSynthAudioProcessorEditor(DirtyLittleBassSynthAudioProcessor&);
    ~DirtyLittleBassSynthAudioProcessorEditor();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    
    OtherLookAndFeel dialLookAndFeel;
    OtherLookAndFeel dryWetLookAndFeel;
    
    TitleHeader titleHeader;
    TitleFooter titleFooter;

    OscVisual oscVisual;
    OscVisual subOscVisual;
    OscVisual lfoVisual;
    
    juce::Rectangle<int> oscVisualSpace;
    
    FilterVisual filterVisual;
    
    juce::Slider oscMorphSlider;
    juce::Slider subMorphSlider;
    juce::Slider subGainSlider;
    juce::Slider pitchBendRangeSlider;
    juce::ComboBox subOctave;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscMorphSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subMorphSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchBendRangeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> subOctaveAttachment;
    
    juce::Label oscMorphLabel;
    juce::Label oscMorphLabel2;
    juce::Label subMorphLabel;
    juce::Label subMorphLabel2;
    juce::Label subGainLabel;
    juce::Label pitchBendRangeLabel;
    
    juce::Slider oscAttackSlider;
    juce::Slider oscDecaySlider;
    juce::Slider oscSustainSlider;
    juce::Slider oscReleaseSlider;
    juce::Slider portaSlider;
    juce::Slider foldbackSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscAttackSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscDecaySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscSustainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscReleaseSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> portaSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> foldbackSliderAttachment;
    
    juce::Label oscAttackLabel;
    juce::Label oscDecayLabel;
    juce::Label oscSustainLabel;
    juce::Label oscReleaseLabel;
    juce::Label portaLabel;
    juce::Label foldbackLabel;
    
    juce::Slider ringToneSlider;
    juce::Slider ringPitchSlider;
    juce::Slider ringDryWetSlider;
    juce::Slider frqShftPitchSlider;
    juce::Slider frqShftDryWetSlider;
    juce::Slider sHPitchSlider;
    juce::Slider sHDryWetSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringToneSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringPitchSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ringDryWetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frqShftPitchSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frqShftDryWetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sHPitchSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sHDryWetSliderAttachment;
    
    juce::Label ringLabel;
    juce::Label frqShftLabel;
    juce::Label sHLabel;
    juce::Label toneLabel;
    juce::Label pitchLabel;
    juce::Label dryWetLabel;
    
    juce::Slider cutoffSlider;
    juce::Slider resSlider;
    juce::ComboBox filterType;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    
    juce::Label cutoffLabel;
    juce::Label resLabel;
    
    juce::Slider fltAttackSlider;
    juce::Slider fltDecaySlider;
    juce::Slider fltSustainSlider;
    juce::Slider fltReleaseSlider;
    juce::Slider adsrToCutoffSlider;
    juce::Slider adsrToResSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fltAttackSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fltDecaySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fltSustainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fltReleaseSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> adsrToCutoffSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> adsrToResSliderAttachment;
    
    juce::Label fltAttackLabel;
    juce::Label fltDecayLabel;
    juce::Label fltSustainLabel;
    juce::Label fltReleaseLabel;
    juce::Label adsrToCutoffLabel;
    juce::Label adsrToResLabel;
    
    juce::Slider lfoShapeSlider;
    juce::Slider lfoFreqSlider;
    juce::Slider lfoAmountSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoShapeSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoFreqSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoAmountSliderAttachment;
    
    juce::Label lfoShapeLabel;
    juce::Label lfoFreqLabel;
    juce::Label lfoAmountLabel;
    
    juce::Slider masterGainSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainSliderAttachment;
    
    juce::Label masterGainLabel;
    
    OutMeter outMeter;
    
    juce::Colour onyx;
    juce::Colour lightSlateGray;
    juce::Colour magicMint;
    juce::Colour fieryRose;
    juce::Colour orangePeel;
    juce::Colour textColor;
    
    juce::Rectangle<float> headerAreaInner;       // Header
    juce::Rectangle<float> mainOutAreaInner;      // Main Out
    juce::Rectangle<float> oscSectionInner;       // Oscillators
    juce::Rectangle<float> mainOscSectionInner;   // Main Oscillator
    juce::Rectangle<float> subOscSectionInner;    // Sub Oscillator
    juce::Rectangle<float> oscADSRSectionInner;   // ADSR
    juce::Rectangle<float> oscADSRSlidersInner;   // ADSR
    juce::Rectangle<float> adsrRotaryInner;       // Porta/Foldback
    juce::Rectangle<float> modSectionInner;       // Modifiers
    juce::Rectangle<float> ringModSectionInner;   // Ring Mod
    juce::Rectangle<float> frqShftSectionInner;   // Freq Shift
    juce::Rectangle<float> sAndHSectionInner;     // S&H
    juce::Rectangle<float> bottomSectionInner;    // Entire Bottom
    juce::Rectangle<float> filterSectionInner;    // Filter
    juce::Rectangle<float> fltADSRSectionInner;   // Filter ADSR
    juce::Rectangle<float> lfoSectionInner;       // LFO
    
    DirtyLittleBassSynthAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirtyLittleBassSynthAudioProcessorEditor)
};
