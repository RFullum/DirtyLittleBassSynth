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
    : AudioProcessorEditor (&p),
        onyx                   ( Colour( (uint8)53,  (uint8)59,  (uint8)60  ) ),
        lightSlateGray         ( Colour( (uint8)130, (uint8)146, (uint8)152 ) ),
        magicMint              ( Colour( (uint8)174, (uint8)255, (uint8)216 ) ),
        fieryRose              ( Colour( (uint8)255, (uint8)104, (uint8)114 ) ),
        orangePeel             ( Colour( (uint8)252, (uint8)152, (uint8)0   ) ),
        processor (p)
        
{
    setSize (1200, 666);
    
    otherLookAndFeel.setColors ( fieryRose, magicMint );
    
    // Master Out
    sliderSetup       ( masterGainSlider, Slider::SliderStyle::LinearVertical, fieryRose, magicMint, true );
    sliderLabelSetup  ( masterGainLabel, "Out Gain", fieryRose );
    addAndMakeVisible ( outMeter );
    
    masterGainSlider.setLookAndFeel ( &otherLookAndFeel );
    
    // Header
    titleHeader.setColors ( magicMint, onyx, orangePeel );
    addAndMakeVisible     ( titleHeader );
    
    // Osc Section
    sliderSetup ( oscMorphSlider,       Slider::SliderStyle::LinearHorizontal, fieryRose, magicMint, false );
    sliderSetup ( subMorphSlider,       Slider::SliderStyle::LinearHorizontal, fieryRose, magicMint, false );
    sliderSetup ( subGainSlider,        Slider::SliderStyle::LinearVertical,   fieryRose, magicMint, false );
    sliderSetup ( pitchBendRangeSlider, Slider::SliderStyle::LinearVertical,   fieryRose, magicMint, true  );
    
    oscMorphSlider.setLookAndFeel       ( &otherLookAndFeel );
    subMorphSlider.setLookAndFeel       ( &otherLookAndFeel );
    subGainSlider.setLookAndFeel        ( &otherLookAndFeel );
    pitchBendRangeSlider.setLookAndFeel ( &otherLookAndFeel );
    
    sliderLabelSetup ( oscMorphLabel,       "Osc\nMorph", fieryRose );
    sliderLabelSetup ( subMorphLabel,       "Sub\nMorph", fieryRose );
    sliderLabelSetup ( subGainLabel,        "Sub Gain",   fieryRose );
    sliderLabelSetup ( pitchBendRangeLabel, "Pitch Bend", fieryRose );
    
    comboBoxSetup ( subOctave, StringArray( {"0", "-1 Oct", "-2 Oct"} ) );
    
    // Osc ADSR Section
    sliderSetup( oscAttackSlider,  Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup( oscDecaySlider,   Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup( oscSustainSlider, Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup( oscReleaseSlider, Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup( portaSlider,      Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup( foldbackSlider,   Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    
    oscAttackSlider.setLookAndFeel  ( &otherLookAndFeel );
    oscDecaySlider.setLookAndFeel   ( &otherLookAndFeel );
    oscSustainSlider.setLookAndFeel ( &otherLookAndFeel );
    oscReleaseSlider.setLookAndFeel ( &otherLookAndFeel );
    portaSlider.setLookAndFeel      ( &otherLookAndFeel );
    foldbackSlider.setLookAndFeel   ( &otherLookAndFeel );
    
    sliderLabelSetup ( oscAttackLabel,  "A",                    fieryRose );
    sliderLabelSetup ( oscDecayLabel,   "D",                    fieryRose );
    sliderLabelSetup ( oscSustainLabel, "S",                    fieryRose );
    sliderLabelSetup ( oscReleaseLabel, "R",                    fieryRose );
    sliderLabelSetup ( portaLabel,      "Portamento",           fieryRose );
    sliderLabelSetup ( foldbackLabel,   "Foldback\nDistortion", fieryRose );
    
    // Modifiers Section
    sliderSetup ( ringToneSlider,      Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup ( ringPitchSlider,     Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup ( ringDryWetSlider,    Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup ( frqShftPitchSlider,  Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup ( frqShftDryWetSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup ( sHPitchSlider,       Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup ( sHDryWetSlider,      Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    
    ringToneSlider.setLookAndFeel      ( &otherLookAndFeel );
    ringPitchSlider.setLookAndFeel     ( &otherLookAndFeel );
    ringDryWetSlider.setLookAndFeel    ( &otherLookAndFeel );
    frqShftPitchSlider.setLookAndFeel  ( &otherLookAndFeel );
    frqShftDryWetSlider.setLookAndFeel ( &otherLookAndFeel );
    sHPitchSlider.setLookAndFeel       ( &otherLookAndFeel );
    sHDryWetSlider.setLookAndFeel      ( &otherLookAndFeel );
    
    sliderLabelSetup ( ringLabel,    "Ring Mod",      fieryRose );
    sliderLabelSetup ( frqShftLabel, "Freq Shift",    fieryRose );
    sliderLabelSetup ( sHLabel,      "Sample & Hold", fieryRose );
    sliderLabelSetup ( toneLabel,    "Tone",          onyx );
    sliderLabelSetup ( pitchLabel,   "Pitch",         onyx );
    sliderLabelSetup ( dryWetLabel,  "Dry/Wet",       onyx );
    
    // Filter Section
    sliderSetup ( cutoffSlider, Slider::SliderStyle::LinearHorizontal, fieryRose, magicMint, false );
    sliderSetup ( resSlider,    Slider::SliderStyle::LinearVertical,   fieryRose, magicMint, false );
    
    cutoffSlider.setLookAndFeel ( &otherLookAndFeel );
    resSlider.setLookAndFeel    ( &otherLookAndFeel );
    
    sliderLabelSetup ( cutoffLabel, "Cutoff", fieryRose );
    sliderLabelSetup ( resLabel, "Resonance", fieryRose );
    
    comboBoxSetup ( filterType, StringArray( {"-12LPF", "-24LPF", "-48LPF", "Notch"} ) );
    
    // Filter ADSR Section
    sliderSetup ( fltAttackSlider,    Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup ( fltDecaySlider,     Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup ( fltSustainSlider,   Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup ( fltReleaseSlider,   Slider::SliderStyle::LinearVertical,               fieryRose, magicMint, true  );
    sliderSetup ( adsrToCutoffSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    sliderSetup ( adsrToResSlider,    Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, magicMint, false );
    
    fltAttackSlider.setLookAndFeel    ( &otherLookAndFeel );
    fltDecaySlider.setLookAndFeel     ( &otherLookAndFeel );
    fltSustainSlider.setLookAndFeel   ( &otherLookAndFeel );
    fltReleaseSlider.setLookAndFeel   ( &otherLookAndFeel );
    adsrToCutoffSlider.setLookAndFeel ( &otherLookAndFeel );
    adsrToResSlider.setLookAndFeel    ( &otherLookAndFeel );
    
    sliderLabelSetup ( fltAttackLabel,    "A",            fieryRose );
    sliderLabelSetup ( fltDecayLabel,     "D",            fieryRose );
    sliderLabelSetup ( fltSustainLabel,   "S",            fieryRose );
    sliderLabelSetup ( fltReleaseLabel,   "R",            fieryRose );
    sliderLabelSetup ( adsrToCutoffLabel, "To Cutoff",    fieryRose );
    sliderLabelSetup ( adsrToResLabel,    "To Resonance", fieryRose );
    
    // Filter LFO Section
    sliderSetup ( lfoShapeSlider,  Slider::SliderStyle::LinearHorizontal, fieryRose, magicMint, false );
    sliderSetup ( lfoFreqSlider,   Slider::SliderStyle::LinearVertical,   fieryRose, magicMint, false );
    sliderSetup ( lfoAmountSlider, Slider::SliderStyle::LinearVertical,   fieryRose, magicMint, false );
    
    lfoShapeSlider.setLookAndFeel  ( &otherLookAndFeel );
    lfoFreqSlider.setLookAndFeel   ( &otherLookAndFeel );
    lfoAmountSlider.setLookAndFeel ( &otherLookAndFeel );
    
    sliderLabelSetup ( lfoShapeLabel,  "LFO Shape", fieryRose );
    sliderLabelSetup ( lfoFreqLabel,   "Frequency", fieryRose );
    sliderLabelSetup ( lfoAmountLabel, "To Cutoff", fieryRose );
    
    
    // Attachments
    oscMorphSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "osc_morph",        oscMorphSlider       );
    subMorphSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "sub_osc_morph",    subMorphSlider       );
    subGainSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "sub_osc_gain",     subGainSlider        );
    pitchBendRangeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "pitch_bend_range", pitchBendRangeSlider );
    subOctaveAttachment      = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment> ( processor.parameters, "sub_osc_octave",   subOctave            );
    
    oscAttackSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_attack",    oscAttackSlider  );
    oscDecaySliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_decay",     oscDecaySlider   );
    oscSustainSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_sustain",   oscSustainSlider );
    oscReleaseSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_release",   oscReleaseSlider );
    portaSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "porta_time",    portaSlider      );
    foldbackSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "foldback_dist", foldbackSlider   );
    
    ringToneSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "ring_tone",        ringToneSlider      );
    ringPitchSliderAttachment     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "ring_mod_pitch",   ringPitchSlider     );
    ringDryWetSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "ring_mod_mix",     ringDryWetSlider    );
    frqShftPitchSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "freq_shift_pitch", frqShftPitchSlider  );
    frqShftDryWetSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "freq_shift_mix",   frqShftDryWetSlider );
    sHPitchSliderAttachment       = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "sandh_pitch",      sHPitchSlider       );
    sHDryWetSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "sandh_mix",        sHDryWetSlider      );
    
    cutoffSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "filter_cutoff", cutoffSlider );
    resSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "filter_res",    resSlider    );
    filterTypeAttachment   = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment> ( processor.parameters, "filter_type",   filterType   );
    
    fltAttackSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_attack",  fltAttackSlider    );
    fltDecaySliderAttachment     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_decay",   fltDecaySlider     );
    fltSustainSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_sustain", fltSustainSlider   );
    fltReleaseSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_release", fltReleaseSlider   );
    adsrToCutoffSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_COAmt",   adsrToCutoffSlider );
    adsrToResSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_ResAmt",  adsrToResSlider    );
    
    lfoShapeSliderAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtLFO_shape", lfoShapeSlider  );
    lfoFreqSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtLFO_freq",  lfoFreqSlider   );
    lfoAmountSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtLFO_amt",   lfoAmountSlider );
    
    masterGainSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "master_gain", masterGainSlider );
    
    // Wave Visual
    oscVisual.setColors    ( magicMint, onyx, fieryRose );
    subOscVisual.setColors ( magicMint, onyx, fieryRose );
    lfoVisual.setColors    ( magicMint, onyx, fieryRose );
    
    filterVisual.setColors ( magicMint, onyx, onyx, fieryRose );
    
    addAndMakeVisible ( oscVisual    );
    addAndMakeVisible ( subOscVisual );
    addAndMakeVisible ( lfoVisual    );
    addAndMakeVisible ( filterVisual );
    
    
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
    float cornerRound = 2.0f;
    
    g.fillAll ( onyx );
    
    // Main Out
    g.setGradientFill      ( ColourGradient::vertical( onyx, mainOutAreaInner.getHeight() * 0.59f,
                                                       magicMint, mainOutAreaInner.getHeight() ));
    g.fillRoundedRectangle ( mainOutAreaInner, cornerRound );
    
    // Oscillators
    g.setColour            ( magicMint );
    g.fillRoundedRectangle ( oscSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( mainOscSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( subOscSectionInner, cornerRound );
    
    // Osc ADSR
    g.setColour            ( magicMint );
    g.fillRoundedRectangle ( oscADSRSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( adsrRotaryInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( oscADSRSlidersInner, cornerRound );
    
    // Modifiers
    g.setColour            ( magicMint );
    g.fillRoundedRectangle ( modSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( ringModSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( frqShftSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( sAndHSectionInner, cornerRound );
    
    // Bottom area
    g.setColour            ( magicMint );
    g.fillRoundedRectangle ( bottomSectionInner, cornerRound );
    
    // Filter
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( filterSectionInner, cornerRound );
    
    // Filter ADSR
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( fltADSRSectionInner, cornerRound );
    
    // LFO
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( lfoSectionInner, cornerRound );
    

}


void DirtyLittleBassSynthAudioProcessorEditor::timerCallback()
{
    oscVisual.setOscShapeLine    ( processor.mainOscVisualBuffer );
    subOscVisual.setOscShapeLine ( processor.subOscVisualBuffer  );
    lfoVisual.setOscShapeLine    ( processor.lfoOscVisualBuffer  );
    
    filterVisual.drawFilterShape ( filterType.getSelectedId(),
                                 (float)cutoffSlider.getValue(),
                                 (float)resSlider.getValue() );
    
    
    float outLevel = ( processor.outputLevelBuffer.getMagnitude( 0, processor.outputLevelBuffer.getNumSamples() ) < 0.001f ) ? 0.0f :
                       processor.outputLevelBuffer.getMagnitude(0, processor.outputLevelBuffer.getNumSamples() );
    
    outMeter.outMeterLevel( outLevel, processor.getSampleRate() );
}


void DirtyLittleBassSynthAudioProcessorEditor::resized()
{
    // Total Area of plugin bounds
    auto totalArea        = getLocalBounds();
    int sectionSpacerSize = 2;                  // Pixels to inset each section from its bounds

    
    // Header space at top for logo & name, etc
    int headerHeight = 66;
    
    Rectangle<int> headerArea    = totalArea.removeFromTop ( headerHeight      );
    Rectangle<int> headerReduced = headerArea.reduced      ( sectionSpacerSize );
    
    headerAreaInner.setBounds( headerReduced.getX(), headerReduced.getY(),
                               headerReduced.getWidth(), headerReduced.getHeight() );
    
    Rectangle<int> titleHeaderArea = Rectangle<int>(headerAreaInner.getX(), headerAreaInner.getY(),
                                                    headerAreaInner.getWidth(), headerAreaInner.getHeight() );
    
    titleHeader.setBounds ( titleHeaderArea );
    
    
    // Output area
    int mainOutWidth    = 100;
    int mainLabelHeight = 30;
    
    Rectangle<int> mainOutArea    = totalArea.removeFromRight ( mainOutWidth      );
    Rectangle<int> mainOutReduced = mainOutArea.reduced       ( sectionSpacerSize );
    
    mainOutAreaInner.setBounds( mainOutReduced.getX(), mainOutReduced.getY(),
                                mainOutReduced.getWidth(), mainOutReduced.getHeight() );
    
    Rectangle<int> meterOutArea     = mainOutReduced.removeFromBottom ( mainOutReduced.getHeight() * 0.5f );
    Rectangle<int> mainOutLabelArea = mainOutReduced.removeFromTop    ( mainLabelHeight );
    
    masterGainLabel.setBounds  ( mainOutLabelArea );
    masterGainSlider.setBounds ( mainOutReduced   );
    outMeter.setBounds         ( meterOutArea     );
    
    
    // Top Row of sections area: Rectangle across top containing
    // Osc Section, ADSR Section, and Modifiers Section
    int topRowHeight = (int)( getHeight() * 0.66f );
    
    Rectangle<int> topSectionArea = totalArea.removeFromTop( topRowHeight );
    
    
    // Oscillator Section area (Top Row subarea)
    int oscSectionWidth = topSectionArea.getWidth() * 0.40f;
    int oscGainWidth    = 75;
    int subOctaveHeight = 30;
    int morphLabelWidth = 50;
    int gainLabelHeight = 30;
    
    // Main Osc Section Area (Oscillator subarea)
    Rectangle<int> oscSection        = topSectionArea.removeFromLeft ( oscSectionWidth );
    Rectangle<int> oscSectionReduced = oscSection.reduced            ( sectionSpacerSize );
    
    oscSectionInner.setBounds ( oscSectionReduced.getX(), oscSectionReduced.getY(),
                                oscSectionReduced.getWidth(), oscSectionReduced.getHeight() );
    
    Rectangle<int> mainOscSection        = oscSectionReduced.removeFromTop ( oscSectionReduced.getHeight() * 0.5f );
    Rectangle<int> mainOscSectionReduced = mainOscSection.reduced          ( sectionSpacerSize * 2.0f );
    
    mainOscSectionInner.setBounds( mainOscSectionReduced.getX(), mainOscSectionReduced.getY(),
                                   mainOscSectionReduced.getWidth(), mainOscSectionReduced.getHeight() );
    
    Rectangle<int> oscGainSpace        = mainOscSectionReduced.removeFromRight  ( oscGainWidth    );
    Rectangle<int> morphLabelSpace     = mainOscSectionReduced.removeFromLeft   ( morphLabelWidth );
    Rectangle<int> morphSliderSpace    = mainOscSectionReduced.removeFromBottom ( gainLabelHeight );
    Rectangle<int> pitchBendLabelSpace = oscGainSpace.removeFromTop             ( gainLabelHeight );
    
    oscVisualSpace = mainOscSectionReduced.reduced( sectionSpacerSize );
    
    oscMorphLabel.setBounds        ( morphLabelSpace     );
    oscMorphSlider.setBounds       ( morphSliderSpace    );
    oscVisual.setBounds            ( oscVisualSpace      );
    pitchBendRangeSlider.setBounds ( oscGainSpace        );
    pitchBendRangeLabel.setBounds  ( pitchBendLabelSpace );
    
    // Sub Osc Section Area (Oscillator subarea)
    Rectangle<int> oscSectionReduced2  = oscSectionReduced.reduced( sectionSpacerSize * 2.0f );
    
    subOscSectionInner.setBounds ( oscSectionReduced2.getX(), oscSectionReduced2.getY(),
                                   oscSectionReduced2.getWidth(), oscSectionReduced2.getHeight() );
    
    Rectangle<int> subGainSpace        = oscSectionReduced2.removeFromRight  ( oscGainWidth    );
    Rectangle<int> subOctaveSpace      = subGainSpace.removeFromBottom       ( subOctaveHeight );
    Rectangle<int> subGainLabelSpace   = subGainSpace.removeFromTop          ( gainLabelHeight );
    Rectangle<int> subMorphLabelSpace  = oscSectionReduced2.removeFromLeft   ( morphLabelWidth );
    Rectangle<int> subMorphSliderSpace = oscSectionReduced2.removeFromBottom ( gainLabelHeight );
    
    subGainLabel.setBounds   ( subGainLabelSpace   );
    subMorphLabel.setBounds  ( subMorphLabelSpace  );
    subOctave.setBounds      ( subOctaveSpace      );
    subGainSlider.setBounds  ( subGainSpace        );
    subMorphSlider.setBounds ( subMorphSliderSpace );
    subOscVisual.setBounds   ( oscSectionReduced2.reduced( sectionSpacerSize ) );
    
    
    // Osc ADSR Section Area
    Rectangle<int> oscADSRSpace        = topSectionArea.removeFromLeft ( topSectionArea.getWidth() * 0.33f );
    Rectangle<int> oscADSRSpaceReduced = oscADSRSpace.reduced          ( sectionSpacerSize );
    
    oscADSRSectionInner.setBounds ( oscADSRSpaceReduced.getX(), oscADSRSpaceReduced.getY(),
                                    oscADSRSpaceReduced.getWidth(), oscADSRSpaceReduced.getHeight() );
    
    int rotaryLabelHeight = 60;
    
    Rectangle<int> rotarySpace = oscADSRSpaceReduced.removeFromBottom ( oscADSRSpaceReduced.getHeight() * 0.33f ).reduced( sectionSpacerSize * 2.0f );
    
    adsrRotaryInner.setBounds ( rotarySpace.getX(), rotarySpace.getY(),
                                rotarySpace.getWidth(), rotarySpace.getHeight() );
    
    Rectangle<int> portaSpace      = rotarySpace.removeFromLeft    ( rotarySpace.getWidth() * 0.5f );
    Rectangle<int> portaLabelSpace = portaSpace.removeFromTop      ( rotaryLabelHeight );
    
    portaSlider.setBounds ( portaSpace );
    portaLabel.setBounds  ( portaLabelSpace );
    
    Rectangle<int> foldbackLabelSpace = rotarySpace.removeFromTop( rotaryLabelHeight );
    
    foldbackSlider.setBounds ( rotarySpace );
    foldbackLabel.setBounds  ( foldbackLabelSpace );
    
    // ADSR Slider Area (Osc ADSR subsection)
    oscADSRSpaceReduced = oscADSRSpaceReduced.reduced( sectionSpacerSize * 2.0f );
    
    oscADSRSlidersInner.setBounds ( oscADSRSpaceReduced.getX(), oscADSRSpaceReduced.getY(),
                                    oscADSRSpaceReduced.getWidth(), oscADSRSpaceReduced.getHeight() );
    
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
    
    modSectionInner.setBounds( modSectionReduced.getX(), modSectionReduced.getY(),
                               modSectionReduced.getWidth(), modSectionReduced.getHeight() );
    
    Rectangle<int> modHeadingsSpace   = modSectionReduced.removeFromTop( modHeadingHeight    );
    Rectangle<int> spacerHeadingSpace = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    Rectangle<int> toneHeadingSpace   = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    Rectangle<int> pitchHeadingSpace  = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    
    toneLabel.setBounds   ( toneHeadingSpace  );
    pitchLabel.setBounds  ( pitchHeadingSpace );
    dryWetLabel.setBounds ( modHeadingsSpace  );
    
    int modKnobHeight = (int)( modSectionReduced.getHeight() * 0.33f );
    
    Rectangle<int> modSubsections = modSectionReduced.reduced( sectionSpacerSize );
    Rectangle<int> ringRowArea    = modSubsections.removeFromTop( modKnobHeight ).reduced( sectionSpacerSize );
    Rectangle<int> frqShftRowArea = modSubsections.removeFromTop( modKnobHeight ).reduced( sectionSpacerSize );
    Rectangle<int> sAndHRowArea   = modSubsections.reduced( sectionSpacerSize );
    
    ringModSectionInner.setBounds ( ringRowArea.getX(), ringRowArea.getY(),
                                    ringRowArea.getWidth(), ringRowArea.getHeight() );
    
    frqShftSectionInner.setBounds ( frqShftRowArea.getX(), frqShftRowArea.getY(),
                                    frqShftRowArea.getWidth(), frqShftRowArea.getHeight() );
    
    sAndHSectionInner.setBounds   ( sAndHRowArea.getX(), sAndHRowArea.getY(),
                                    sAndHRowArea.getWidth(), sAndHRowArea.getHeight() );
    
    Rectangle<int> modTypeSection   = modSectionReduced.removeFromLeft ( modSectionGridWidth );
    Rectangle<int> ringLabelArea    = modTypeSection.removeFromTop     ( modKnobHeight       );
    Rectangle<int> frqShftLabelArea = modTypeSection.removeFromTop     ( modKnobHeight       );
    
    ringLabel.setBounds    ( ringLabelArea    );
    frqShftLabel.setBounds ( frqShftLabelArea );
    sHLabel.setBounds      ( modTypeSection   );
    
    Rectangle<int> ringKnobArea      = modSectionReduced.removeFromTop ( modKnobHeight       );
    Rectangle<int> ringToneKnobArea  = ringKnobArea.removeFromLeft     ( modSectionGridWidth );
    Rectangle<int> ringPitchKnobArea = ringKnobArea.removeFromLeft     ( modSectionGridWidth );
    
    ringToneSlider.setBounds   ( ringToneKnobArea  );
    ringPitchSlider.setBounds  ( ringPitchKnobArea );
    ringDryWetSlider.setBounds ( ringKnobArea      );
    
    Rectangle<int> frqShKnobArea = modSectionReduced.removeFromTop ( modKnobHeight );
    Rectangle<int> frqShSpacer   = frqShKnobArea.removeFromLeft    ( modSectionGridWidth );
    Rectangle<int> frqShPtchArea = frqShKnobArea.removeFromLeft    ( modSectionGridWidth );
    
    frqShftPitchSlider.setBounds  ( frqShPtchArea );
    frqShftDryWetSlider.setBounds ( frqShKnobArea );
    
    Rectangle<int> sHSpacer    = modSectionReduced.removeFromLeft( modSectionGridWidth );
    Rectangle<int> sHPitchArea = modSectionReduced.removeFromLeft( modSectionGridWidth );
    
    sHPitchSlider.setBounds  ( sHPitchArea       );
    sHDryWetSlider.setBounds ( modSectionReduced );
    
    
    // Bottom Section: Rectangle across bottom containing Filter Section,
    // Filter ADSR Section, and Filter LFO section
    auto bottomSectionArea = totalArea.reduced( sectionSpacerSize );
    
    bottomSectionInner.setBounds( bottomSectionArea.getX(), bottomSectionArea.getY(),
                                  bottomSectionArea.getWidth(), bottomSectionArea.getHeight() );
    
    int lowerGridWidth  = (int)( bottomSectionArea.getWidth() * 0.33f );
    int filtLabelheight = 30;
    
    
    // Filter Area
    Rectangle<int> filterArea        = bottomSectionArea.removeFromLeft ( lowerGridWidth    );
    Rectangle<int> filterAreaReduced = filterArea.reduced               ( sectionSpacerSize );
    
    filterSectionInner.setBounds ( filterAreaReduced.getX(), filterAreaReduced.getY(),
                                   filterAreaReduced.getWidth(), filterAreaReduced.getHeight() );
    
    int resWidth = (int)(filterAreaReduced.getWidth() * 0.25f);
    
    Rectangle<int> resArea         = filterAreaReduced.removeFromRight  ( resWidth        );
    Rectangle<int> resLableArea    = resArea.removeFromTop              ( filtLabelheight );
    Rectangle<int> cOLabelArea     = filterAreaReduced.removeFromLeft   ( morphLabelWidth );
    Rectangle<int> fltTypeArea     = filterAreaReduced.removeFromBottom ( filtLabelheight ).removeFromRight( 75 );
    Rectangle<int> fltCOSliderArea = filterAreaReduced.removeFromBottom ( filtLabelheight );
    
    resLabel.setBounds     ( resLableArea      );
    resSlider.setBounds    ( resArea           );
    filterType.setBounds   ( fltTypeArea       );
    cutoffLabel.setBounds  ( cOLabelArea       );
    cutoffSlider.setBounds ( fltCOSliderArea   );
    filterVisual.setBounds ( filterAreaReduced );
    
    // Filter ADSR Area
    Rectangle<int> fltADSRArea           = bottomSectionArea.removeFromLeft ( lowerGridWidth );
    Rectangle<int> fltADSRAreaReduced    = fltADSRArea.reduced              ( sectionSpacerSize );
    
    fltADSRSectionInner.setBounds ( fltADSRAreaReduced.getX(), fltADSRAreaReduced.getY(),
                                    fltADSRAreaReduced.getWidth(), fltADSRAreaReduced.getHeight() );
    
    Rectangle<int> fltADSRRotaryArea     = fltADSRAreaReduced.removeFromRight ( (int)(fltADSRAreaReduced.getWidth() * 0.33f) );
    Rectangle<int> fltADSRToResArea      = fltADSRRotaryArea.removeFromBottom ( (int)(fltADSRRotaryArea.getHeight() * 0.5f) );
    Rectangle<int> fltADSRToResLabelArea = fltADSRToResArea.removeFromTop     ( filtLabelheight );
    Rectangle<int> fltADSRToCOLabelArea  = fltADSRRotaryArea.removeFromTop    ( filtLabelheight );
    
    adsrToResLabel.setBounds     ( fltADSRToResLabelArea );
    adsrToResSlider.setBounds    ( fltADSRToResArea      );
    adsrToCutoffLabel.setBounds  ( fltADSRToCOLabelArea  );
    adsrToCutoffSlider.setBounds ( fltADSRRotaryArea     );
    
    int fltADSRWidth = (int)(fltADSRAreaReduced.getWidth() * 0.25f);
    
    Rectangle<int> fltADSRHeader = fltADSRAreaReduced.removeFromTop ( filtLabelheight );
    Rectangle<int> fltALabelArea = fltADSRHeader.removeFromLeft     ( fltADSRWidth    );
    Rectangle<int> fltDLabelArea = fltADSRHeader.removeFromLeft     ( fltADSRWidth    );
    Rectangle<int> fltSLabelArea = fltADSRHeader.removeFromLeft     ( fltADSRWidth    );
    
    fltAttackLabel.setBounds  ( fltALabelArea );
    fltDecayLabel.setBounds   ( fltDLabelArea );
    fltSustainLabel.setBounds ( fltSLabelArea );
    fltReleaseLabel.setBounds ( fltADSRHeader );
    
    Rectangle<int> fltASliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    Rectangle<int> fltDSliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    Rectangle<int> fltSSliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    
    fltAttackSlider.setBounds  ( fltASliderArea     );
    fltDecaySlider.setBounds   ( fltDSliderArea     );
    fltSustainSlider.setBounds ( fltSSliderArea     );
    fltReleaseSlider.setBounds ( fltADSRAreaReduced );
    
    // LFO Area
    auto lfoArea = bottomSectionArea;
    
    Rectangle<int> lfoAreaReduced = lfoArea.reduced( sectionSpacerSize );
    
    lfoSectionInner.setBounds( lfoAreaReduced.getX(), lfoAreaReduced.getY(), lfoAreaReduced.getWidth(), lfoAreaReduced.getHeight() );
    
    Rectangle<int> lfoVertSliderArea    = lfoAreaReduced.removeFromRight      ( (int)( lfoAreaReduced.getWidth() * 0.33f ) );
    Rectangle<int> lfoVertLabelFootArea = lfoVertSliderArea.removeFromTop     ( filtLabelheight );
    Rectangle<int> lfoFrqLabelArea      = lfoVertLabelFootArea.removeFromLeft ( (int)( lfoVertLabelFootArea.getWidth() * 0.5f ) );
    
    lfoFreqLabel.setBounds   ( lfoFrqLabelArea      );
    lfoAmountLabel.setBounds ( lfoVertLabelFootArea );
    
    Rectangle<int> lfoFreqSliderArea = lfoVertSliderArea.removeFromLeft( (int)( lfoVertSliderArea.getWidth() * 0.5f ) );
    
    lfoFreqSlider.setBounds   ( lfoFreqSliderArea );
    lfoAmountSlider.setBounds ( lfoVertSliderArea );
    
    Rectangle<int> lfoShapeArea       = lfoAreaReduced.removeFromBottom( filtLabelheight );
    Rectangle<int> lfoShapeSliderArea = lfoAreaReduced.removeFromBottom( filtLabelheight );
    
    lfoShapeLabel.setBounds  ( lfoShapeArea       );
    lfoShapeSlider.setBounds ( lfoShapeSliderArea );
    lfoVisual.setBounds      ( lfoAreaReduced     );
    
    
}

void DirtyLittleBassSynthAudioProcessorEditor::sliderSetup(Slider& sliderInstance, Slider::SliderStyle style,
                                                           Colour& sliderFillColor, Colour& sliderThumbColor,
                                                           bool showTextBox)
{
    sliderInstance.setSliderStyle(style);
    
    // If slider has a textbox, draw it, otherwise, don't
    if (showTextBox)
    {
        sliderInstance.setTextBoxStyle ( Slider::TextBoxBelow, false, 40, 20 );
        sliderInstance.setColour       ( Slider::textBoxOutlineColourId, Colour( (uint8)0, (uint8)0, (uint8)0, (uint8)0 ) );
        sliderInstance.setColour       ( Slider::textBoxTextColourId, orangePeel );
    }
    else
    {
        sliderInstance.setTextBoxStyle( Slider::NoTextBox, false, 0, 0 );
    }
    
    // Slider types
    if (sliderInstance.getSliderStyle() == Slider::SliderStyle::LinearHorizontal)
    {
        sliderInstance.setColour( Slider::trackColourId, sliderFillColor );
    }
    else if (sliderInstance.getSliderStyle() == Slider::SliderStyle::LinearVertical)
    {
        sliderInstance.setColour( Slider::trackColourId, sliderFillColor );
    }
    else if (sliderInstance.getSliderStyle() == Slider::SliderStyle::RotaryHorizontalVerticalDrag)
    {
        sliderInstance.setColour( Slider::rotarySliderFillColourId, sliderFillColor );
    }
    
    sliderInstance.setColour( Slider::thumbColourId, sliderThumbColor );
    
    addAndMakeVisible(sliderInstance);
}


void DirtyLittleBassSynthAudioProcessorEditor::sliderLabelSetup(Label& labelInstance, String labelText, Colour& c)
{
    labelInstance.setFont              ( Font("helvetica", 12.0f, 1) );
    labelInstance.setText              ( labelText, dontSendNotification );
    labelInstance.setJustificationType ( Justification::centred );
    labelInstance.setColour            ( Label::textColourId, c );
    
    addAndMakeVisible(labelInstance);
}

void DirtyLittleBassSynthAudioProcessorEditor::comboBoxSetup(ComboBox& boxInstance, StringArray boxItems)
{
    boxInstance.addItemList          ( boxItems, 1 );
    boxInstance.setJustificationType ( Justification::centred );
    boxInstance.setSelectedItemIndex ( 0 );
    addAndMakeVisible                ( boxInstance );
}
