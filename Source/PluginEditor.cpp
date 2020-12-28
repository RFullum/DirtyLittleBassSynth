/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Wavetable5AudioProcessorEditor::Wavetable5AudioProcessorEditor (Wavetable5AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (1000, 700);
    
    /*
    addAndMakeVisible(testSlider1);
    testSlider1.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    testSlider1.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    
    testSlider1.setLookAndFeel(&otherLookAndFeel);
    */
    
    /*
    addAndMakeVisible(testSlider2);
    testSlider2.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    testSlider2.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    
    testSlider2.setLookAndFeel(&otherLookAndFeel);
    */
    
    /*
    addAndMakeVisible(testSlider3);
    testSlider3.setSliderStyle(Slider::SliderStyle::ThreeValueHorizontal);
    */
    
    // Master Out
    addAndMakeVisible(masterGainSlider);
    masterGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    masterGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    
    // Osc Section
    addAndMakeVisible(oscMorphSlider);
    oscMorphSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    oscMorphSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    
    addAndMakeVisible(subMorphSlider);
    subMorphSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    subMorphSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    
}

Wavetable5AudioProcessorEditor::~Wavetable5AudioProcessorEditor()
{
}

//==============================================================================
void Wavetable5AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void Wavetable5AudioProcessorEditor::resized()
{
    //testSlider1.setBounds(getLocalBounds());
    //testSlider2.setBounds( getLocalBounds() );
    //testSlider3.setBounds( getLocalBounds() );
    auto totalArea = getLocalBounds();
    
    int mainOutWidth = 100;
    Rectangle<int> mainOutArea = totalArea.removeFromRight( mainOutWidth );
    masterGainSlider.setBounds( mainOutArea );
    
    int topRowHeight = 400;
    Rectangle<int> topSectionArea = totalArea.removeFromTop( topRowHeight );
    
    int oscSectionWidth = 200;
    Rectangle<int> oscSection = topSectionArea.removeFromLeft( oscSectionWidth );
    Rectangle<int> mainOscSection = oscSection.removeFromTop( oscSection.getHeight() * 0.5f );
    oscMorphSlider.setBounds( mainOscSection );
    subMorphSlider.setBounds( oscSection );
    
}
