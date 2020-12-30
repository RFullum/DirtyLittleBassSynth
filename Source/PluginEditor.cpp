/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DirtyLittleBassSynthAudioProcessorEditor::DirtyLittleBassSynthAudioProcessorEditor (DirtyLittleBassSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
        pluginBackground( Colour( (uint8)255, (uint8)64, (uint8)0 ) ),
        headerColor( Colour( (uint8)77, (uint8)125, (uint8)184 ) ),
        mainOutColour( Colour( (uint8)35, (uint8)25, (uint8)204 ) ),
        oscSectionColour( Colour( (uint8)100, (uint8)55, (uint8)109 ) )
{
    setSize (1200, 666);
    
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
    sliderSetup( masterGainSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    
    sliderLabelSetup( masterGainLabel, "Out Gain" );
    
    // Osc Section
    sliderSetup( oscMorphSlider, Slider::SliderStyle::LinearHorizontal, Colours::blue );
    sliderSetup( subMorphSlider, Slider::SliderStyle::LinearHorizontal, Colours::blue );
    sliderSetup( subGainSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    
    sliderLabelSetup( oscMorphLabel, "Osc\nMorph" );
    sliderLabelSetup( subMorphLabel, "Sub\nMorph" );
    sliderLabelSetup( subGainLabel, "Sub\nGain" );
    
    comboBoxSetup(subOctave, StringArray( {"0", "-1 Oct", "-2 Oct"} ) );
    
    // Osc ADSR Section
    sliderSetup( oscAttackSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( oscDecaySlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( oscSustainSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( oscReleaseSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( portaSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( foldbackSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    
    sliderLabelSetup( oscAttackLabel, "A" );
    sliderLabelSetup( oscDecayLabel, "D" );
    sliderLabelSetup( oscSustainLabel, "S" );
    sliderLabelSetup( oscReleaseLabel, "R" );
    sliderLabelSetup( portaLabel, "Portamento" );
    sliderLabelSetup( foldbackLabel, "Foldback\nDistortion" );
    
    // Modifiers Section
    sliderSetup( ringToneSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( ringPitchSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( ringDryWetSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( frqShftPitchSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( frqShftDryWetSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( sHPitchSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( sHDryWetSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    
    sliderLabelSetup( ringLabel, "Ring Mod" );
    sliderLabelSetup( frqShftLabel, "Freq Shift" );
    sliderLabelSetup( sHLabel, "Sample & Hold" );
    sliderLabelSetup( toneLabel, "Tone" );
    sliderLabelSetup( pitchLabel, "Pitch" );
    sliderLabelSetup( dryWetLabel, "Dry/Wet" );
    
    // Filter Section
    sliderSetup( cutoffSlider, Slider::SliderStyle::LinearHorizontal, Colours::blue );
    sliderSetup( resSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    
    sliderLabelSetup( cutoffLabel, "Cutoff" );
    sliderLabelSetup( resLabel, "Resonance" );
    
    comboBoxSetup(filterType, StringArray( {"-12LPF", "-24LPF", "-48LPF", "Notch"} ) );
    
    // Filter ADSR Section
    sliderSetup( fltAttackSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( fltDecaySlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( fltSustainSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( fltReleaseSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( adsrToCutoffSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    sliderSetup( adsrToResSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, Colours::blue );
    
    sliderLabelSetup( fltAttackLabel, "A" );
    sliderLabelSetup( fltDecayLabel, "D" );
    sliderLabelSetup( fltSustainLabel, "S" );
    sliderLabelSetup( fltReleaseLabel, "R" );
    sliderLabelSetup( adsrToCutoffLabel, "To Cutoff" );
    sliderLabelSetup( adsrToResLabel, "To Resonance" );
    
    // Filter LFO Section
    sliderSetup( lfoShapeSlider, Slider::SliderStyle::LinearHorizontal, Colours::blue );
    sliderSetup( lfoFreqSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    sliderSetup( lfoAmountSlider, Slider::SliderStyle::LinearVertical, Colours::blue );
    
    sliderLabelSetup( lfoShapeLabel, "LFO Shape" );
    sliderLabelSetup( lfoFreqLabel, "Frequency" );
    sliderLabelSetup( lfoAmountLabel, "To Cutoff" );
    
    
    // Attachments
    oscMorphSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "osc_morph", oscMorphSlider);
    subMorphSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "sub_osc_morph", subMorphSlider);
    subGainSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "sub_osc_gain", subGainSlider);
    subOctaveAttachment      = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "sub_osc_octave", subOctave);
    
    oscAttackSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "amp_attack", oscAttackSlider);
    oscDecaySliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "amp_decay", oscDecaySlider);
    oscSustainSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "amp_sustain", oscSustainSlider);
    oscReleaseSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "amp_release", oscReleaseSlider);
    portaSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "porta_time", portaSlider);
    foldbackSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "foldback_dist", foldbackSlider);
    
    ringToneSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "ring_tone", ringToneSlider);
    ringPitchSliderAttachment     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "ring_mod_pitch", ringPitchSlider);
    ringDryWetSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "ring_mod_mix", ringDryWetSlider);
    frqShftPitchSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "freq_shift_pitch", frqShftPitchSlider);
    frqShftDryWetSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "freq_shift_mix", frqShftDryWetSlider);
    sHPitchSliderAttachment       = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "sandh_pitch", sHPitchSlider);
    sHDryWetSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "sandh_mix", sHDryWetSlider);
    
    cutoffSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filter_cutoff", cutoffSlider);
    resSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filter_res", resSlider);
    filterTypeAttachment   = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "filter_type", filterType);
    
    fltAttackSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtEnv_attack", fltAttackSlider);
    fltDecaySliderAttachment     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtEnv_decay", fltDecaySlider);
    fltSustainSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtEnv_sustain", fltSustainSlider);
    fltReleaseSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtEnv_release", fltReleaseSlider);
    adsrToCutoffSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtEnv_COAmt", adsrToCutoffSlider);
    adsrToResSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtEnv_ResAmt", adsrToResSlider);
    
    lfoShapeSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtLFO_shape", lfoShapeSlider);
    lfoFreqSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtLFO_freq", lfoFreqSlider);
    lfoAmountSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "filtLFO_amt", lfoAmountSlider);
    
    masterGainSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "master_gain", masterGainSlider);
    
    // Wave Visual
    addAndMakeVisible(oscVisual);
    addAndMakeVisible(subOscVisual);
    addAndMakeVisible(lfoVisual);
    
    
    // Timer
    Timer::startTimerHz(60);
}

DirtyLittleBassSynthAudioProcessorEditor::~DirtyLittleBassSynthAudioProcessorEditor()
{
    Timer::stopTimer();
}

//==============================================================================
void DirtyLittleBassSynthAudioProcessorEditor::paint (Graphics& g)
{
    float cornerRound = 10.0f;
    
    g.fillAll (pluginBackground);
    
    g.setColour            ( headerColor );
    g.fillRoundedRectangle ( headerAreaInner, cornerRound );
    
    g.setColour            ( mainOutColour );
    g.fillRoundedRectangle ( mainOutAreaInner, cornerRound );
    
    g.setColour            ( oscSectionColour );
    g.fillRoundedRectangle ( oscSectionInner, cornerRound );
    
    g.setColour            ( headerColor );
    g.fillRoundedRectangle ( oscADSRSectionInner, cornerRound );
    
    g.setColour            ( mainOutColour );
    g.fillRoundedRectangle ( modSectionInner, cornerRound );
    
    g.setColour            ( headerColor );
    g.fillRoundedRectangle ( filterSectionInner, cornerRound );
    
    g.setColour            ( oscSectionColour );
    g.fillRoundedRectangle ( fltADSRSectionInner, cornerRound );
    
    g.setColour            ( mainOutColour );
    g.fillRoundedRectangle ( lfoSectionInner, cornerRound );

}

void DirtyLittleBassSynthAudioProcessorEditor::timerCallback()
{
    oscVisual.setOscShapeLine(processor.mainOscVisualBuffer);
    subOscVisual.setOscShapeLine(processor.subOscVisualBuffer);
    lfoVisual.setOscShapeLine(processor.lfoOscVisualBuffer);
}

void DirtyLittleBassSynthAudioProcessorEditor::resized()
{
    // Total Area of plugin bounds
    auto totalArea        = getLocalBounds();
    int sectionSpacerSize = 2;                  // Pixels to inset each section from its bounds

    
    // Header space at top for logo & name, etc
    int headerHeight = 66;
    
    Rectangle<int> headerArea    = totalArea.removeFromTop ( headerHeight );
    Rectangle<int> headerReduced = headerArea.reduced( sectionSpacerSize );
    
    //headerAreaInner.setSize( headerReduced.getWidth(), headerReduced.getHeight() );        // Reduced headerArea by sectionSpacerSize
    headerAreaInner.setBounds( headerReduced.getX(), headerReduced.getY(), headerReduced.getWidth(), headerReduced.getHeight() );
    
    
    // Output area
    int mainOutWidth = 100;
    int mainLabelHeight = 30;
    
    Rectangle<int> mainOutArea    = totalArea.removeFromRight    ( mainOutWidth );
    Rectangle<int> mainOutReduced = mainOutArea.reduced( sectionSpacerSize );
    
    //mainOutAreaInner.setSize( mainOutReduced.getWidth(), mainOutReduced.getHeight() );
    mainOutAreaInner.setBounds( mainOutReduced.getX(), mainOutReduced.getY(), mainOutReduced.getWidth(), mainOutReduced.getHeight() );
    
    Rectangle<int> mainOutLabelArea = mainOutReduced.removeFromTop ( mainLabelHeight );
    
    masterGainLabel.setBounds  ( mainOutLabelArea );
    masterGainSlider.setBounds ( mainOutReduced );
    
    
    // Top Row of sections area: Rectangle across top containing
    // Osc Section, ADSR Section, and Modifiers Section
    int topRowHeight = (int)(getHeight() * 0.66f);
    
    Rectangle<int> topSectionArea = totalArea.removeFromTop( topRowHeight );
    
    
    // Oscillator Section area (Top Row subarea)
    int oscSectionWidth = topSectionArea.getWidth() * 0.33f;
    int oscGainWidth    = 55;
    int subOctaveHeight = 30;
    int morphLabelWidth = 50;
    int gainLabelHeight = 30;
    
    // Main Osc Section Area (Oscillator subarea)
    Rectangle<int> oscSection        = topSectionArea.removeFromLeft ( oscSectionWidth );
    Rectangle<int> oscSectionReduced = oscSection.reduced            ( sectionSpacerSize );
    
    //oscSectionInner.setSize( oscSectionReduced.getWidth(), oscSectionReduced.getHeight() );
    oscSectionInner.setBounds( oscSectionReduced.getX(), oscSectionReduced.getY(), oscSectionReduced.getWidth(), oscSectionReduced.getHeight() );
    
    Rectangle<int> mainOscSection   = oscSectionReduced.removeFromTop ( oscSectionReduced.getHeight() * 0.5f );
    Rectangle<int> oscGainSpace     = mainOscSection.removeFromRight  ( oscGainWidth );
    Rectangle<int> morphLabelSpace  = mainOscSection.removeFromLeft   ( morphLabelWidth );
    Rectangle<int> morphSliderSpace = mainOscSection.removeFromBottom ( gainLabelHeight );
    
    oscVisualSpace = mainOscSection;
    
    oscMorphLabel.setBounds  ( morphLabelSpace );
    oscMorphSlider.setBounds ( morphSliderSpace );
    oscVisual.setBounds      ( mainOscSection );
    
    // Sub Osc Section Area (Oscillator subarea)
    Rectangle<int> subGainSpace        = oscSectionReduced.removeFromRight  ( oscGainWidth );
    Rectangle<int> subOctaveSpace      = subGainSpace.removeFromBottom      ( subOctaveHeight );
    Rectangle<int> subGainLabelSpace   = subGainSpace.removeFromTop         ( gainLabelHeight );
    Rectangle<int> subMorphLabelSpace  = oscSectionReduced.removeFromLeft   ( morphLabelWidth );
    Rectangle<int> subMorphSliderSpace = oscSectionReduced.removeFromBottom ( gainLabelHeight );
    
    subGainLabel.setBounds   ( subGainLabelSpace );
    subMorphLabel.setBounds  ( subMorphLabelSpace );
    subOctave.setBounds      ( subOctaveSpace );
    subGainSlider.setBounds  ( subGainSpace );
    subMorphSlider.setBounds ( subMorphSliderSpace );
    subOscVisual.setBounds   ( oscSectionReduced );
    
    
    // Osc ADSR Section Area
    Rectangle<int> oscADSRSpace        = topSectionArea.removeFromLeft( topSectionArea.getWidth() * 0.33f );
    Rectangle<int> oscADSRSpaceReduced = oscADSRSpace.reduced( sectionSpacerSize );
    
    //oscADSRSectionInner.setSize( oscADSRSpaceReduced.getWidth(), oscADSRSpaceReduced.getHeight() );
    oscADSRSectionInner.setBounds( oscADSRSpaceReduced.getX(), oscADSRSpaceReduced.getY(), oscADSRSpaceReduced.getWidth(), oscADSRSpaceReduced.getHeight() );
    
    int rotaryLabelHeight = 60;
    
    Rectangle<int> rotarySpace     = oscADSRSpaceReduced.removeFromBottom ( oscADSRSpaceReduced.getHeight() * 0.5f );
    Rectangle<int> portaSpace      = rotarySpace.removeFromLeft    ( rotarySpace.getWidth() * 0.5f );
    Rectangle<int> portaLabelSpace = portaSpace.removeFromTop      ( rotaryLabelHeight );
    
    portaSlider.setBounds( portaSpace );
    portaLabel.setBounds( portaLabelSpace );
    
    Rectangle<int> foldbackLabelSpace = rotarySpace.removeFromTop( rotaryLabelHeight );
    
    foldbackSlider.setBounds ( rotarySpace );
    foldbackLabel.setBounds  ( foldbackLabelSpace );
    
    // ADSR Slider Area (Osc ADSR subsection)
    int oscADSRSliderWidth = oscADSRSpaceReduced.getWidth() * 0.25f;
    
    Rectangle<int> oscASpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    Rectangle<int> oscDSpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    Rectangle<int> oscSSpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    Rectangle<int> oscRSpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    
    // ADSR Labels Area (Osc ADSR subsection)
    int oscADSRLableHeight = 30;
    
    Rectangle<int> oscALabelSpace = oscASpace.removeFromTop( oscADSRLableHeight );
    Rectangle<int> oscDLabelSpace = oscDSpace.removeFromTop( oscADSRLableHeight );
    Rectangle<int> oscSLabelSpace = oscSSpace.removeFromTop( oscADSRLableHeight );
    Rectangle<int> oscRLabelSpace = oscRSpace.removeFromTop( oscADSRLableHeight );
    
    oscAttackSlider.setBounds  ( oscASpace );
    oscDecaySlider.setBounds   ( oscDSpace );
    oscSustainSlider.setBounds ( oscSSpace );
    oscReleaseSlider.setBounds ( oscRSpace );
    
    oscAttackLabel.setBounds  ( oscALabelSpace );
    oscDecayLabel.setBounds   ( oscDLabelSpace );
    oscSustainLabel.setBounds ( oscSLabelSpace );
    oscReleaseLabel.setBounds ( oscRLabelSpace );
    
    
    // Modifiers Area
    int modHeadingHeight    = 30;
    int modSectionGridWidth = (int)(topSectionArea.getWidth() * 0.25f);
    
    Rectangle<int> modSectionSpace   = topSectionArea;
    Rectangle<int> modSectionReduced = modSectionSpace.reduced ( sectionSpacerSize );
    
    //modSectionInner.setSize( modSectionReduced.getWidth(), modSectionReduced.getHeight() );
    modSectionInner.setBounds( modSectionReduced.getX(), modSectionReduced.getY(), modSectionReduced.getWidth(), modSectionReduced.getHeight() );
    
    Rectangle<int> modHeadingsSpace   = modSectionReduced.removeFromTop( modHeadingHeight );
    Rectangle<int> spacerHeadingSpace = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    Rectangle<int> toneHeadingSpace   = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    Rectangle<int> pitchHeadingSpace  = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    
    toneLabel.setBounds   ( toneHeadingSpace );
    pitchLabel.setBounds  ( pitchHeadingSpace );
    dryWetLabel.setBounds ( modHeadingsSpace );
    
    int modKnobHeight = (int)( modSectionReduced.getHeight() * 0.33f );
    
    Rectangle<int> modTypeSection   = modSectionReduced.removeFromLeft ( modSectionGridWidth );
    Rectangle<int> ringLabelArea    = modTypeSection.removeFromTop     ( modKnobHeight );
    Rectangle<int> frqShftLabelArea = modTypeSection.removeFromTop     ( modKnobHeight );
    
    ringLabel.setBounds    ( ringLabelArea );
    frqShftLabel.setBounds ( frqShftLabelArea );
    sHLabel.setBounds      ( modTypeSection );
    
    Rectangle<int> ringKnobArea      = modSectionReduced.removeFromTop ( modKnobHeight );
    Rectangle<int> ringToneKnobArea  = ringKnobArea.removeFromLeft  ( modSectionGridWidth );
    Rectangle<int> ringPitchKnobArea = ringKnobArea.removeFromLeft  ( modSectionGridWidth );
    
    ringToneSlider.setBounds( ringToneKnobArea );
    ringPitchSlider.setBounds( ringPitchKnobArea );
    ringDryWetSlider.setBounds( ringKnobArea );
    
    Rectangle<int> frqShKnobArea = modSectionReduced.removeFromTop( modKnobHeight );
    Rectangle<int> frqShSpacer   = frqShKnobArea.removeFromLeft( modSectionGridWidth );
    Rectangle<int> frqShPtchArea = frqShKnobArea.removeFromLeft( modSectionGridWidth );
    
    frqShftPitchSlider.setBounds( frqShPtchArea );
    frqShftDryWetSlider.setBounds( frqShKnobArea );
    
    Rectangle<int> sHSpacer    = modSectionReduced.removeFromLeft( modSectionGridWidth );
    Rectangle<int> sHPitchArea = modSectionReduced.removeFromLeft( modSectionGridWidth );
    
    sHPitchSlider.setBounds( sHPitchArea );
    sHDryWetSlider.setBounds( modSectionReduced );
    
    
    // Bottom Section: Rectangle across bottom containing Filter Section,
    // Filter ADSR Section, and Filter LFO section
    auto bottomSectionArea = totalArea;
    
    int lowerGridWidth  = (int)(bottomSectionArea.getWidth() * 0.33f);
    int filtLabelheight = 30;
    
    
    // Filter Area
    Rectangle<int> filterArea        = bottomSectionArea.removeFromLeft( lowerGridWidth );
    Rectangle<int> filterAreaReduced = filterArea.reduced( sectionSpacerSize );
    
    //filterSectionInner.setSize( filterAreaReduced.getWidth(), filterAreaReduced.getHeight() );
    filterSectionInner.setBounds( filterAreaReduced.getX(), filterAreaReduced.getY(), filterAreaReduced.getWidth(), filterAreaReduced.getHeight() );
    
    int resWidth = (int)(filterAreaReduced.getWidth() * 0.25f);
    
    Rectangle<int> resArea      = filterAreaReduced.removeFromRight  ( resWidth );
    Rectangle<int> resLableArea = resArea.removeFromTop       ( filtLabelheight );
    Rectangle<int> cOLabelArea  = filterAreaReduced.removeFromLeft ( morphLabelWidth );
    Rectangle<int> fltTypeArea  = filterAreaReduced.removeFromBottom( filtLabelheight ).removeFromRight(50);//cOLabelArea.removeFromRight(50);
    
    resLabel.setBounds( resLableArea );
    resSlider.setBounds( resArea );
    filterType.setBounds( fltTypeArea );
    cutoffLabel.setBounds( cOLabelArea );
    cutoffSlider.setBounds( filterAreaReduced );
    
    
    // Filter ADSR Area
    Rectangle<int> fltADSRArea           = bottomSectionArea.removeFromLeft ( lowerGridWidth );
    Rectangle<int> fltADSRAreaReduced    = fltADSRArea.reduced              ( sectionSpacerSize );
    
    //fltADSRSectionInner.setSize( fltADSRAreaReduced.getWidth(), fltADSRAreaReduced.getHeight() );
    fltADSRSectionInner.setBounds( fltADSRAreaReduced.getX(), fltADSRAreaReduced.getY(), fltADSRAreaReduced.getWidth(), fltADSRAreaReduced.getHeight() );
    
    Rectangle<int> fltADSRRotaryArea     = fltADSRAreaReduced.removeFromRight        ( (int)(fltADSRAreaReduced.getWidth() * 0.33f) );
    Rectangle<int> fltADSRToResArea      = fltADSRRotaryArea.removeFromBottom ( (int)(fltADSRRotaryArea.getHeight() * 0.5f) );
    Rectangle<int> fltADSRToResLabelArea = fltADSRToResArea.removeFromTop     ( filtLabelheight );
    Rectangle<int> fltADSRToCOLabelArea  = fltADSRRotaryArea.removeFromTop    ( filtLabelheight );
    
    adsrToResLabel.setBounds( fltADSRToResLabelArea );
    adsrToResSlider.setBounds( fltADSRToResArea );
    adsrToCutoffLabel.setBounds( fltADSRToCOLabelArea );
    adsrToCutoffSlider.setBounds( fltADSRRotaryArea );
    
    int fltADSRWidth = (int)(fltADSRAreaReduced.getWidth() * 0.25f);
    
    Rectangle<int> fltADSRHeader = fltADSRAreaReduced.removeFromTop    ( filtLabelheight );
    Rectangle<int> fltALabelArea = fltADSRHeader.removeFromLeft ( fltADSRWidth );
    Rectangle<int> fltDLabelArea = fltADSRHeader.removeFromLeft ( fltADSRWidth );
    Rectangle<int> fltSLabelArea = fltADSRHeader.removeFromLeft ( fltADSRWidth );
    
    fltAttackLabel.setBounds( fltALabelArea );
    fltDecayLabel.setBounds( fltDLabelArea );
    fltSustainLabel.setBounds( fltSLabelArea );
    fltReleaseLabel.setBounds( fltADSRHeader );
    
    Rectangle<int> fltASliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    Rectangle<int> fltDSliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    Rectangle<int> fltSSliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    
    fltAttackSlider.setBounds( fltASliderArea );
    fltDecaySlider.setBounds( fltDSliderArea );
    fltSustainSlider.setBounds( fltSSliderArea );
    fltReleaseSlider.setBounds( fltADSRAreaReduced );
    
    // LFO Area
    auto lfoArea = bottomSectionArea;
    
    Rectangle<int> lfoAreaReduced = lfoArea.reduced( sectionSpacerSize );
    
    //lfoSectionInner.setSize( lfoAreaReduced.getWidth(), lfoAreaReduced.getHeight() );
    lfoSectionInner.setBounds( lfoAreaReduced.getX(), lfoAreaReduced.getY(), lfoAreaReduced.getWidth(), lfoAreaReduced.getHeight() );
    
    Rectangle<int> lfoVertSliderArea    = lfoAreaReduced.removeFromRight             ( (int)(lfoAreaReduced.getWidth() * 0.33f) );
    Rectangle<int> lfoVertLabelFootArea = lfoVertSliderArea.removeFromTop     ( filtLabelheight );
    Rectangle<int> lfoFrqLabelArea      = lfoVertLabelFootArea.removeFromLeft ( (int)(lfoVertLabelFootArea.getWidth() * 0.5f) );
    
    lfoFreqLabel.setBounds( lfoFrqLabelArea );
    lfoAmountLabel.setBounds( lfoVertLabelFootArea );
    
    Rectangle<int> lfoFreqSliderArea = lfoVertSliderArea.removeFromLeft( (int)(lfoVertSliderArea.getWidth() * 0.5f) );
    
    lfoFreqSlider.setBounds( lfoFreqSliderArea );
    lfoAmountSlider.setBounds( lfoVertSliderArea );
    
    Rectangle<int> lfoShapeArea = lfoAreaReduced.removeFromBottom( filtLabelheight );
    Rectangle<int> lfoShapeSliderArea = lfoAreaReduced.removeFromBottom( filtLabelheight );
    
    lfoShapeLabel.setBounds( lfoShapeArea );
    lfoShapeSlider.setBounds( lfoShapeSliderArea );
    lfoVisual.setBounds( lfoAreaReduced );
    
    
}

void DirtyLittleBassSynthAudioProcessorEditor::sliderSetup(Slider& sliderInstance, Slider::SliderStyle style, Colour sliderFillColor)
{
    sliderInstance.setSliderStyle(style);
    sliderInstance.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    
    if (sliderInstance.getSliderStyle() == Slider::SliderStyle::LinearHorizontal)
    {
        sliderInstance.setColour(Slider::trackColourId, sliderFillColor);
    }
    else if (sliderInstance.getSliderStyle() == Slider::SliderStyle::LinearVertical)
    {
        sliderInstance.setColour(Slider::trackColourId, sliderFillColor);
    }
    else if (sliderInstance.getSliderStyle() == Slider::SliderStyle::RotaryHorizontalVerticalDrag)
    {
        sliderInstance.setColour(Slider::rotarySliderFillColourId, sliderFillColor);
    }
    
    addAndMakeVisible(sliderInstance);
}


void DirtyLittleBassSynthAudioProcessorEditor::sliderLabelSetup(Label& labelInstance, String labelText)
{
    labelInstance.setText(labelText, dontSendNotification);
    labelInstance.setJustificationType(Justification::centred);
    addAndMakeVisible(labelInstance);
}

void DirtyLittleBassSynthAudioProcessorEditor::comboBoxSetup(ComboBox& boxInstance, StringArray boxItems)
{
    boxInstance.addItemList(boxItems, 1);
    boxInstance.setJustificationType(Justification::centred);
    boxInstance.setSelectedItemIndex(0);
    addAndMakeVisible(boxInstance);
}
