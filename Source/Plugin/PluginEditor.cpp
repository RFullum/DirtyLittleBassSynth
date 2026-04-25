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
    : juce::AudioProcessorEditor (&p),
        onyx                   ( juce::Colour( (juce::uint8)53,  (juce::uint8)59,  (juce::uint8)60  ) ),
        lightSlateGray         ( juce::Colour( (juce::uint8)130, (juce::uint8)146, (juce::uint8)152 ) ),
        magicMint              ( juce::Colour( (juce::uint8)174, (juce::uint8)255, (juce::uint8)216 ) ),
        fieryRose              ( juce::Colour( (juce::uint8)255, (juce::uint8)104, (juce::uint8)114 ) ),
        orangePeel             ( juce::Colour( (juce::uint8)252, (juce::uint8)152, (juce::uint8)0   ) ),
        textColor              ( juce::Colour( (juce::uint8)255, (juce::uint8)255, (juce::uint8)255 ) ),
        processor (p)
        
{
    setSize (1300, 700);
    
    dialLookAndFeel.setColors   ( fieryRose, onyx, magicMint  );
    dryWetLookAndFeel.setColors ( orangePeel, onyx, magicMint );
    
    // Master Out
    sliderSetup        ( masterGainSlider, juce::Slider::SliderStyle::LinearVertical, orangePeel, orangePeel, true );
    sliderLabelSetup   ( masterGainLabel, "Out Gain", textColor, 16.0f );
    outMeter.setColors ( magicMint, fieryRose );
    addAndMakeVisible  ( outMeter );
    
    masterGainSlider.setLookAndFeel ( &dialLookAndFeel );
    
    // Header
    titleHeader.setColors ( magicMint, onyx, orangePeel );
    titleFooter.setColors ( magicMint, onyx, orangePeel );
    
    addAndMakeVisible ( titleHeader );
    addAndMakeVisible ( titleFooter );
    
    // Osc Section
    sliderSetup ( oscMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, magicMint, magicMint,   false );
    sliderSetup ( subMorphSlider,       juce::Slider::SliderStyle::LinearHorizontal, magicMint, magicMint,   false );
    sliderSetup ( subGainSlider,        juce::Slider::SliderStyle::LinearVertical,   orangePeel, orangePeel, false );
    sliderSetup ( pitchBendRangeSlider, juce::Slider::SliderStyle::LinearVertical,   orangePeel, orangePeel, true  );
    
    oscMorphSlider.setLookAndFeel       ( &dialLookAndFeel );
    subMorphSlider.setLookAndFeel       ( &dialLookAndFeel );
    subGainSlider.setLookAndFeel        ( &dialLookAndFeel );
    pitchBendRangeSlider.setLookAndFeel ( &dialLookAndFeel );
    
    sliderLabelSetup ( oscMorphLabel,       "OSC",         textColor, 18.0f );
    sliderLabelSetup ( oscMorphLabel2,      "MORPH",       textColor, 15.0f );
    sliderLabelSetup ( subMorphLabel,       "SUB",         textColor, 18.0f );
    sliderLabelSetup ( subMorphLabel2,      "MORPH",       textColor, 15.0f );
    sliderLabelSetup ( subGainLabel,        "Sub Gain",    textColor, 13.0f );
    sliderLabelSetup ( pitchBendRangeLabel, "Bend\nRange", textColor, 13.0f );
    
    comboBoxSetup ( subOctave, juce::StringArray( {"0", "-1 Oct", "-2 Oct"} ) );
    
    // Osc juce::ADSR Section
    sliderSetup( oscAttackSlider,  juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup( oscDecaySlider,   juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup( oscSustainSlider, juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup( oscReleaseSlider, juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup( portaSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup( foldbackSlider,   juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    
    oscAttackSlider.setLookAndFeel  ( &dialLookAndFeel );
    oscDecaySlider.setLookAndFeel   ( &dialLookAndFeel );
    oscSustainSlider.setLookAndFeel ( &dialLookAndFeel );
    oscReleaseSlider.setLookAndFeel ( &dialLookAndFeel );
    portaSlider.setLookAndFeel      ( &dialLookAndFeel );
    foldbackSlider.setLookAndFeel   ( &dialLookAndFeel );
    
    sliderLabelSetup ( oscAttackLabel,  "A",                    textColor, 18.0f );
    sliderLabelSetup ( oscDecayLabel,   "D",                    textColor, 18.0f );
    sliderLabelSetup ( oscSustainLabel, "S",                    textColor, 18.0f );
    sliderLabelSetup ( oscReleaseLabel, "R",                    textColor, 18.0f );
    sliderLabelSetup ( portaLabel,      "Portamento",           textColor, 17.0f );
    sliderLabelSetup ( foldbackLabel,   "Foldback\nDistortion", textColor, 17.0f );
    
    // Modifiers Section
    sliderSetup ( ringToneSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup ( ringPitchSlider,     juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup ( ringDryWetSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup ( frqShftPitchSlider,  juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup ( frqShftDryWetSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup ( sHPitchSlider,       juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup ( sHDryWetSlider,      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    
    ringToneSlider.setLookAndFeel      ( &dialLookAndFeel   );
    ringPitchSlider.setLookAndFeel     ( &dialLookAndFeel   );
    ringDryWetSlider.setLookAndFeel    ( &dryWetLookAndFeel );
    frqShftPitchSlider.setLookAndFeel  ( &dialLookAndFeel   );
    frqShftDryWetSlider.setLookAndFeel ( &dryWetLookAndFeel );
    sHPitchSlider.setLookAndFeel       ( &dialLookAndFeel   );
    sHDryWetSlider.setLookAndFeel      ( &dryWetLookAndFeel );
    
    sliderLabelSetup ( ringLabel,    "Ring Mod",      textColor, 14.0f );
    sliderLabelSetup ( frqShftLabel, "Freq Shift",    textColor, 14.0f );
    sliderLabelSetup ( sHLabel,      "Sample & Hold", textColor, 14.0f );
    sliderLabelSetup ( toneLabel,    "Tone",          onyx, 16.0f );
    sliderLabelSetup ( pitchLabel,   "Pitch",         onyx, 16.0f );
    sliderLabelSetup ( dryWetLabel,  "Dry/Wet",       onyx, 16.0f );
    
    // Filter Section
    sliderSetup ( cutoffSlider, juce::Slider::SliderStyle::LinearHorizontal, magicMint, magicMint, false );
    sliderSetup ( resSlider,    juce::Slider::SliderStyle::LinearVertical,   magicMint, magicMint, false );
    
    cutoffSlider.setLookAndFeel ( &dialLookAndFeel );
    resSlider.setLookAndFeel    ( &dialLookAndFeel );
    
    sliderLabelSetup ( cutoffLabel, "Cutoff", textColor, 15.0f );
    sliderLabelSetup ( resLabel,    "Rez",    textColor, 15.0f );
    
    comboBoxSetup ( filterType, juce::StringArray( {"-12LPF", "-24LPF", "-48LPF", "Notch"} ) );
    
    // Filter juce::ADSR Section
    sliderSetup ( fltAttackSlider,    juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup ( fltDecaySlider,     juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup ( fltSustainSlider,   juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup ( fltReleaseSlider,   juce::Slider::SliderStyle::LinearVertical,               fieryRose, fieryRose, true  );
    sliderSetup ( adsrToCutoffSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    sliderSetup ( adsrToResSlider,    juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, fieryRose, fieryRose, false );
    
    fltAttackSlider.setLookAndFeel    ( &dialLookAndFeel   );
    fltDecaySlider.setLookAndFeel     ( &dialLookAndFeel   );
    fltSustainSlider.setLookAndFeel   ( &dialLookAndFeel   );
    fltReleaseSlider.setLookAndFeel   ( &dialLookAndFeel   );
    adsrToCutoffSlider.setLookAndFeel ( &dryWetLookAndFeel );
    adsrToResSlider.setLookAndFeel    ( &dryWetLookAndFeel );
    
    sliderLabelSetup ( fltAttackLabel,    "A",            textColor, 17.0f );
    sliderLabelSetup ( fltDecayLabel,     "D",            textColor, 17.0f );
    sliderLabelSetup ( fltSustainLabel,   "S",            textColor, 17.0f );
    sliderLabelSetup ( fltReleaseLabel,   "R",            textColor, 17.0f );
    sliderLabelSetup ( adsrToCutoffLabel, "To Cutoff",    textColor, 15.0f );
    sliderLabelSetup ( adsrToResLabel,    "To Rez",       textColor, 15.0f );
    
    // Filter LFO Section
    sliderSetup ( lfoShapeSlider,  juce::Slider::SliderStyle::LinearHorizontal, magicMint,  magicMint, false  );
    sliderSetup ( lfoFreqSlider,   juce::Slider::SliderStyle::LinearVertical,   fieryRose,  fieryRose, false  );
    sliderSetup ( lfoAmountSlider, juce::Slider::SliderStyle::LinearVertical,   orangePeel, orangePeel, false );
    
    lfoShapeSlider.setLookAndFeel  ( &dialLookAndFeel );
    lfoFreqSlider.setLookAndFeel   ( &dialLookAndFeel );
    lfoAmountSlider.setLookAndFeel ( &dialLookAndFeel );
    
    sliderLabelSetup ( lfoShapeLabel,  "LFO Shape", textColor, 17.0f );
    sliderLabelSetup ( lfoFreqLabel,   "Freq",      textColor, 16.0f );
    sliderLabelSetup ( lfoAmountLabel, "Amount",    textColor, 16.0f );
    
    
    // Attachments
    oscMorphSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "osc_morph",        oscMorphSlider       );
    subMorphSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "sub_osc_morph",    subMorphSlider       );
    subGainSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "sub_osc_gain",     subGainSlider        );
    pitchBendRangeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "pitch_bend_range", pitchBendRangeSlider );
    subOctaveAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ( processor.parameters, "sub_osc_octave",   subOctave            );
    
    oscAttackSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_attack",    oscAttackSlider  );
    oscDecaySliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_decay",     oscDecaySlider   );
    oscSustainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_sustain",   oscSustainSlider );
    oscReleaseSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "amp_release",   oscReleaseSlider );
    portaSliderAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "porta_time",    portaSlider      );
    foldbackSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "foldback_dist", foldbackSlider   );
    
    ringToneSliderAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "ring_tone",        ringToneSlider      );
    ringPitchSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "ring_mod_pitch",   ringPitchSlider     );
    ringDryWetSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "ring_mod_mix",     ringDryWetSlider    );
    frqShftPitchSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "freq_shift_pitch", frqShftPitchSlider  );
    frqShftDryWetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "freq_shift_mix",   frqShftDryWetSlider );
    sHPitchSliderAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "sandh_pitch",      sHPitchSlider       );
    sHDryWetSliderAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "sandh_mix",        sHDryWetSlider      );
    
    cutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "filter_cutoff", cutoffSlider );
    resSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>   ( processor.parameters, "filter_res",    resSlider    );
    filterTypeAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ( processor.parameters, "filter_type",   filterType   );
    
    fltAttackSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_attack",  fltAttackSlider    );
    fltDecaySliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_decay",   fltDecaySlider     );
    fltSustainSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_sustain", fltSustainSlider   );
    fltReleaseSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_release", fltReleaseSlider   );
    adsrToCutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_COAmt",   adsrToCutoffSlider );
    adsrToResSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtEnv_ResAmt",  adsrToResSlider    );
    
    lfoShapeSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtLFO_shape", lfoShapeSlider  );
    lfoFreqSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtLFO_freq",  lfoFreqSlider   );
    lfoAmountSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "filtLFO_amt",   lfoAmountSlider );
    
    masterGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> ( processor.parameters, "master_gain", masterGainSlider );
    
    // Wave Visual
    juce::Colour onyx2 = onyx.darker().darker();
    
    oscVisual.setColors    ( magicMint, onyx, onyx2 );
    subOscVisual.setColors ( magicMint, onyx, onyx2 );
    lfoVisual.setColors    ( magicMint, onyx, onyx2 );
    
    filterVisual.setColors ( magicMint, onyx2, onyx, onyx2 );
    
    addAndMakeVisible ( oscVisual    );
    addAndMakeVisible ( subOscVisual );
    addAndMakeVisible ( lfoVisual    );
    addAndMakeVisible ( filterVisual );
    
    
    // juce::Timer
    juce::Timer::startTimerHz(60);
}

DirtyLittleBassSynthAudioProcessorEditor::~DirtyLittleBassSynthAudioProcessorEditor()
{
    juce::Timer::stopTimer();
}

//==============================================================================
void DirtyLittleBassSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    float cornerRound = 2.0f;
    
    g.fillAll ( onyx );
    
    // Main Out
    g.setGradientFill      ( juce::ColourGradient::vertical( onyx, mainOutAreaInner.getHeight() * 0.59f,
                                                       magicMint, mainOutAreaInner.getHeight() ));
    g.fillRoundedRectangle ( mainOutAreaInner, cornerRound );
    
    // Oscillators
    g.setColour            ( magicMint );
    g.fillRoundedRectangle ( oscSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( mainOscSectionInner, cornerRound );
    
    g.setColour            ( onyx );
    g.fillRoundedRectangle ( subOscSectionInner, cornerRound );
    
    // Osc juce::ADSR
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
    
    // Filter juce::ADSR
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
    
    juce::Rectangle<int> headerArea    = totalArea.removeFromTop ( headerHeight      );
    juce::Rectangle<int> headerReduced = headerArea.reduced      ( sectionSpacerSize );
    
    headerAreaInner.setBounds( headerReduced.getX(), headerReduced.getY(),
                               headerReduced.getWidth(), headerReduced.getHeight() );
    
    juce::Rectangle<int> titleHeaderArea = juce::Rectangle<int>(headerAreaInner.getX(), headerAreaInner.getY(),
                                                    headerAreaInner.getWidth(), headerAreaInner.getHeight() );
    
    titleHeader.setBounds ( titleHeaderArea );
    
    juce::Rectangle<int> footerArea = totalArea.removeFromBottom( 17 );
    
    titleFooter.setBounds( footerArea );
    
    
    // Output area
    int mainOutWidth    = 100;
    int mainLabelHeight = 30;
    
    juce::Rectangle<int> mainOutArea    = totalArea.removeFromRight ( mainOutWidth      );
    juce::Rectangle<int> mainOutReduced = mainOutArea.reduced       ( sectionSpacerSize );
    
    mainOutAreaInner.setBounds( mainOutReduced.getX(), mainOutReduced.getY(),
                                mainOutReduced.getWidth(), mainOutReduced.getHeight() );
    
    juce::Rectangle<int> meterOutArea     = mainOutReduced.removeFromBottom ( mainOutReduced.getHeight() * 0.5f );
    juce::Rectangle<int> mainOutLabelArea = mainOutReduced.removeFromTop    ( mainLabelHeight );
    
    masterGainLabel.setBounds  ( mainOutLabelArea );
    masterGainSlider.setBounds ( mainOutReduced   );
    outMeter.setBounds         ( meterOutArea     );
    
    
    // Top Row of sections area: juce::Rectangle across top containing
    // Osc Section, juce::ADSR Section, and Modifiers Section
    int topRowHeight = (int)( getHeight() * 0.66f );
    
    juce::Rectangle<int> topSectionArea = totalArea.removeFromTop( topRowHeight );
    
    
    // Oscillator Section area (Top Row subarea)
    int oscSectionWidth = topSectionArea.getWidth() * 0.40f;
    int oscGainWidth    = 75;
    int subOctaveHeight = 30;
    int morphLabelWidth = 50;
    int gainLabelHeight = 30;
    
    // Main Osc Section Area (Oscillator subarea)
    juce::Rectangle<int> oscSection        = topSectionArea.removeFromLeft ( oscSectionWidth );
    juce::Rectangle<int> oscSectionReduced = oscSection.reduced            ( sectionSpacerSize );
    
    oscSectionInner.setBounds ( oscSectionReduced.getX(), oscSectionReduced.getY(),
                                oscSectionReduced.getWidth(), oscSectionReduced.getHeight() );
    
    juce::Rectangle<int> mainOscSection        = oscSectionReduced.removeFromTop ( oscSectionReduced.getHeight() * 0.5f );
    juce::Rectangle<int> mainOscSectionReduced = mainOscSection.reduced          ( sectionSpacerSize * 2.0f );
    
    mainOscSectionInner.setBounds( mainOscSectionReduced.getX(), mainOscSectionReduced.getY(),
                                   mainOscSectionReduced.getWidth(), mainOscSectionReduced.getHeight() );
    
    juce::Rectangle<int> oscGainSpace         = mainOscSectionReduced.removeFromRight  ( oscGainWidth    );
    juce::Rectangle<int> morphLabelSpace      = mainOscSectionReduced.removeFromLeft   ( morphLabelWidth );
    juce::Rectangle<int> morphLabelSpace2     = morphLabelSpace.removeFromTop          ( morphLabelSpace.getHeight() * 0.5f );
    juce::Rectangle<int> morphOSCLabelSpace   = morphLabelSpace2.removeFromBottom      ( gainLabelHeight - 10.0f );
    juce::Rectangle<int> morphMORPHLabelSpace = morphLabelSpace.removeFromTop          ( gainLabelHeight - 10.0f );
    juce::Rectangle<int> morphSliderSpace     = mainOscSectionReduced.removeFromBottom ( gainLabelHeight );
    juce::Rectangle<int> pitchBendLabelSpace  = oscGainSpace.removeFromTop             ( gainLabelHeight );
    
    oscVisualSpace = mainOscSectionReduced.reduced( sectionSpacerSize );
    
    oscMorphLabel.setBounds        ( morphOSCLabelSpace   );
    oscMorphLabel2.setBounds       ( morphMORPHLabelSpace );
    oscMorphSlider.setBounds       ( morphSliderSpace     );
    oscVisual.setBounds            ( oscVisualSpace       );
    pitchBendRangeSlider.setBounds ( oscGainSpace         );
    pitchBendRangeLabel.setBounds  ( pitchBendLabelSpace  );
    
    // Sub Osc Section Area (Oscillator subarea)
    juce::Rectangle<int> oscSectionReduced2  = oscSectionReduced.reduced( sectionSpacerSize * 2.0f );
    
    subOscSectionInner.setBounds ( oscSectionReduced2.getX(), oscSectionReduced2.getY(),
                                   oscSectionReduced2.getWidth(), oscSectionReduced2.getHeight() );
    
    juce::Rectangle<int> subGainSpace            = oscSectionReduced2.removeFromRight   ( oscGainWidth    );
    juce::Rectangle<int> subOctaveSpace          = subGainSpace.removeFromBottom        ( subOctaveHeight );
    juce::Rectangle<int> subGainLabelSpace       = subGainSpace.removeFromTop           ( gainLabelHeight );
    juce::Rectangle<int> subMorphLabelSpace      = oscSectionReduced2.removeFromLeft    ( morphLabelWidth );
    juce::Rectangle<int> subMorphLabelSpace2     = subMorphLabelSpace.removeFromTop     ( subMorphLabelSpace.getHeight() * 0.5f );
    juce::Rectangle<int> subMorphOSCLabelSpace   = subMorphLabelSpace2.removeFromBottom ( gainLabelHeight - 10.0f );
    juce::Rectangle<int> subMorphMORPHLabelSpace = subMorphLabelSpace.removeFromTop     ( gainLabelHeight - 10.0f );
    juce::Rectangle<int> subMorphSliderSpace     = oscSectionReduced2.removeFromBottom  ( gainLabelHeight );
    
    subGainLabel.setBounds   ( subGainLabelSpace       );
    subMorphLabel.setBounds  ( subMorphOSCLabelSpace   );
    subMorphLabel2.setBounds ( subMorphMORPHLabelSpace );
    subOctave.setBounds      ( subOctaveSpace          );
    subGainSlider.setBounds  ( subGainSpace            );
    subMorphSlider.setBounds ( subMorphSliderSpace     );
    subOscVisual.setBounds   ( oscSectionReduced2.reduced( sectionSpacerSize ) );
    
    
    // Osc juce::ADSR Section Area
    juce::Rectangle<int> oscADSRSpace        = topSectionArea.removeFromLeft ( topSectionArea.getWidth() * 0.33f );
    juce::Rectangle<int> oscADSRSpaceReduced = oscADSRSpace.reduced          ( sectionSpacerSize );
    
    oscADSRSectionInner.setBounds ( oscADSRSpaceReduced.getX(), oscADSRSpaceReduced.getY(),
                                    oscADSRSpaceReduced.getWidth(), oscADSRSpaceReduced.getHeight() );
    
    int rotaryLabelHeight = 60;
    
    juce::Rectangle<int> rotarySpace = oscADSRSpaceReduced.removeFromBottom ( oscADSRSpaceReduced.getHeight() * 0.33f ).reduced( sectionSpacerSize * 2.0f );
    
    adsrRotaryInner.setBounds ( rotarySpace.getX(), rotarySpace.getY(),
                                rotarySpace.getWidth(), rotarySpace.getHeight() );
    
    juce::Rectangle<int> portaSpace      = rotarySpace.removeFromLeft    ( rotarySpace.getWidth() * 0.5f );
    juce::Rectangle<int> portaLabelSpace = portaSpace.removeFromTop      ( rotaryLabelHeight );
    
    portaSlider.setBounds ( portaSpace );
    portaLabel.setBounds  ( portaLabelSpace );
    
    juce::Rectangle<int> foldbackLabelSpace = rotarySpace.removeFromTop( rotaryLabelHeight );
    
    foldbackSlider.setBounds ( rotarySpace );
    foldbackLabel.setBounds  ( foldbackLabelSpace );
    
    // juce::ADSR juce::Slider Area (Osc juce::ADSR subsection)
    oscADSRSpaceReduced = oscADSRSpaceReduced.reduced( sectionSpacerSize * 2.0f );
    
    oscADSRSlidersInner.setBounds ( oscADSRSpaceReduced.getX(), oscADSRSpaceReduced.getY(),
                                    oscADSRSpaceReduced.getWidth(), oscADSRSpaceReduced.getHeight() );
    
    int oscADSRSliderWidth = oscADSRSpaceReduced.getWidth() * 0.25f;
    
    juce::Rectangle<int> oscASpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    juce::Rectangle<int> oscDSpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    juce::Rectangle<int> oscSSpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    juce::Rectangle<int> oscRSpace = oscADSRSpaceReduced.removeFromLeft( oscADSRSliderWidth );
    
    // juce::ADSR Labels Area (Osc juce::ADSR subsection)
    int oscADSRLableHeight = 30;
    
    juce::Rectangle<int> oscALabelSpace = oscASpace.removeFromTop( oscADSRLableHeight );
    juce::Rectangle<int> oscDLabelSpace = oscDSpace.removeFromTop( oscADSRLableHeight );
    juce::Rectangle<int> oscSLabelSpace = oscSSpace.removeFromTop( oscADSRLableHeight );
    juce::Rectangle<int> oscRLabelSpace = oscRSpace.removeFromTop( oscADSRLableHeight );
    
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
    
    juce::Rectangle<int> modSectionSpace   = topSectionArea;
    juce::Rectangle<int> modSectionReduced = modSectionSpace.reduced ( sectionSpacerSize );
    
    modSectionInner.setBounds( modSectionReduced.getX(), modSectionReduced.getY(),
                               modSectionReduced.getWidth(), modSectionReduced.getHeight() );
    
    juce::Rectangle<int> modHeadingsSpace   = modSectionReduced.removeFromTop( modHeadingHeight    );
    juce::Rectangle<int> spacerHeadingSpace = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    juce::Rectangle<int> toneHeadingSpace   = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    juce::Rectangle<int> pitchHeadingSpace  = modHeadingsSpace.removeFromLeft( modSectionGridWidth );
    
    toneLabel.setBounds   ( toneHeadingSpace  );
    pitchLabel.setBounds  ( pitchHeadingSpace );
    dryWetLabel.setBounds ( modHeadingsSpace  );
    
    int modKnobHeight = (int)( modSectionReduced.getHeight() * 0.33f );
    
    juce::Rectangle<int> modSubsections = modSectionReduced.reduced( sectionSpacerSize );
    juce::Rectangle<int> ringRowArea    = modSubsections.removeFromTop( modKnobHeight ).reduced( sectionSpacerSize );
    juce::Rectangle<int> frqShftRowArea = modSubsections.removeFromTop( modKnobHeight ).reduced( sectionSpacerSize );
    juce::Rectangle<int> sAndHRowArea   = modSubsections.reduced( sectionSpacerSize );
    
    ringModSectionInner.setBounds ( ringRowArea.getX(), ringRowArea.getY(),
                                    ringRowArea.getWidth(), ringRowArea.getHeight() );
    
    frqShftSectionInner.setBounds ( frqShftRowArea.getX(), frqShftRowArea.getY(),
                                    frqShftRowArea.getWidth(), frqShftRowArea.getHeight() );
    
    sAndHSectionInner.setBounds   ( sAndHRowArea.getX(), sAndHRowArea.getY(),
                                    sAndHRowArea.getWidth(), sAndHRowArea.getHeight() );
    
    juce::Rectangle<int> modTypeSection   = modSectionReduced.removeFromLeft ( modSectionGridWidth );
    juce::Rectangle<int> ringLabelArea    = modTypeSection.removeFromTop     ( modKnobHeight       );
    juce::Rectangle<int> frqShftLabelArea = modTypeSection.removeFromTop     ( modKnobHeight       );
    
    ringLabel.setBounds    ( ringLabelArea    );
    frqShftLabel.setBounds ( frqShftLabelArea );
    sHLabel.setBounds      ( modTypeSection   );
    
    juce::Rectangle<int> ringKnobArea      = modSectionReduced.removeFromTop ( modKnobHeight       );
    juce::Rectangle<int> ringToneKnobArea  = ringKnobArea.removeFromLeft     ( modSectionGridWidth );
    juce::Rectangle<int> ringPitchKnobArea = ringKnobArea.removeFromLeft     ( modSectionGridWidth );
    
    ringToneSlider.setBounds   ( ringToneKnobArea  );
    ringPitchSlider.setBounds  ( ringPitchKnobArea );
    ringDryWetSlider.setBounds ( ringKnobArea      );
    
    juce::Rectangle<int> frqShKnobArea = modSectionReduced.removeFromTop ( modKnobHeight );
    juce::Rectangle<int> frqShSpacer   = frqShKnobArea.removeFromLeft    ( modSectionGridWidth );
    juce::Rectangle<int> frqShPtchArea = frqShKnobArea.removeFromLeft    ( modSectionGridWidth );
    
    frqShftPitchSlider.setBounds  ( frqShPtchArea );
    frqShftDryWetSlider.setBounds ( frqShKnobArea );
    
    juce::Rectangle<int> sHSpacer    = modSectionReduced.removeFromLeft( modSectionGridWidth );
    juce::Rectangle<int> sHPitchArea = modSectionReduced.removeFromLeft( modSectionGridWidth );
    
    sHPitchSlider.setBounds  ( sHPitchArea       );
    sHDryWetSlider.setBounds ( modSectionReduced );
    
    
    // Bottom Section: juce::Rectangle across bottom containing Filter Section,
    // Filter juce::ADSR Section, and Filter LFO section
    auto bottomSectionArea = totalArea.reduced( sectionSpacerSize );
    
    bottomSectionInner.setBounds( bottomSectionArea.getX(), bottomSectionArea.getY(),
                                  bottomSectionArea.getWidth(), bottomSectionArea.getHeight() );
    
    int lowerGridWidth  = (int)( bottomSectionArea.getWidth() * 0.33f );
    int filtLabelheight = 30;
    
    
    // Filter Area
    juce::Rectangle<int> filterArea        = bottomSectionArea.removeFromLeft ( lowerGridWidth    );
    juce::Rectangle<int> filterAreaReduced = filterArea.reduced               ( sectionSpacerSize );
    
    filterSectionInner.setBounds ( filterAreaReduced.getX(), filterAreaReduced.getY(),
                                   filterAreaReduced.getWidth(), filterAreaReduced.getHeight() );
    
    int resWidth = (int)(filterAreaReduced.getWidth() * 0.125f);
    
    juce::Rectangle<int> resArea         = filterAreaReduced.removeFromRight  ( resWidth        );
    juce::Rectangle<int> resLableArea    = resArea.removeFromTop              ( filtLabelheight );
    juce::Rectangle<int> cOLabelArea     = filterAreaReduced.removeFromLeft   ( morphLabelWidth );
    juce::Rectangle<int> fltTypeArea     = filterAreaReduced.removeFromBottom ( filtLabelheight ).removeFromRight( 75 );
    juce::Rectangle<int> fltCOSliderArea = filterAreaReduced.removeFromBottom ( filtLabelheight );
    
    resLabel.setBounds     ( resLableArea      );
    resSlider.setBounds    ( resArea           );
    filterType.setBounds   ( fltTypeArea       );
    cutoffLabel.setBounds  ( cOLabelArea.removeFromBottom( cOLabelArea.getHeight() * 0.5f )       );
    cutoffSlider.setBounds ( fltCOSliderArea   );
    filterVisual.setBounds ( filterAreaReduced );
    
    // Filter juce::ADSR Area
    juce::Rectangle<int> fltADSRArea           = bottomSectionArea.removeFromLeft ( lowerGridWidth );
    juce::Rectangle<int> fltADSRAreaReduced    = fltADSRArea.reduced              ( sectionSpacerSize );
    
    fltADSRSectionInner.setBounds ( fltADSRAreaReduced.getX(), fltADSRAreaReduced.getY(),
                                    fltADSRAreaReduced.getWidth(), fltADSRAreaReduced.getHeight() );
    
    juce::Rectangle<int> fltADSRRotaryArea     = fltADSRAreaReduced.removeFromRight ( (int)(fltADSRAreaReduced.getWidth() * 0.33f) );
    juce::Rectangle<int> fltADSRToResArea      = fltADSRRotaryArea.removeFromBottom ( (int)(fltADSRRotaryArea.getHeight() * 0.5f) );
    juce::Rectangle<int> fltADSRToResLabelArea = fltADSRToResArea.removeFromTop     ( filtLabelheight - 9.0f );
    juce::Rectangle<int> fltADSRToCOLabelArea  = fltADSRRotaryArea.removeFromTop    ( filtLabelheight - 9.0f );
    
    adsrToResLabel.setBounds     ( fltADSRToResLabelArea );
    adsrToResSlider.setBounds    ( fltADSRToResArea      );
    adsrToCutoffLabel.setBounds  ( fltADSRToCOLabelArea  );
    adsrToCutoffSlider.setBounds ( fltADSRRotaryArea     );
    
    int fltADSRWidth = (int)(fltADSRAreaReduced.getWidth() * 0.25f);
    
    juce::Rectangle<int> fltADSRHeader = fltADSRAreaReduced.removeFromTop ( filtLabelheight );
    juce::Rectangle<int> fltALabelArea = fltADSRHeader.removeFromLeft     ( fltADSRWidth    );
    juce::Rectangle<int> fltDLabelArea = fltADSRHeader.removeFromLeft     ( fltADSRWidth    );
    juce::Rectangle<int> fltSLabelArea = fltADSRHeader.removeFromLeft     ( fltADSRWidth    );
    
    fltAttackLabel.setBounds  ( fltALabelArea );
    fltDecayLabel.setBounds   ( fltDLabelArea );
    fltSustainLabel.setBounds ( fltSLabelArea );
    fltReleaseLabel.setBounds ( fltADSRHeader );
    
    juce::Rectangle<int> fltASliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    juce::Rectangle<int> fltDSliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    juce::Rectangle<int> fltSSliderArea = fltADSRAreaReduced.removeFromLeft( fltADSRWidth );
    
    fltAttackSlider.setBounds  ( fltASliderArea     );
    fltDecaySlider.setBounds   ( fltDSliderArea     );
    fltSustainSlider.setBounds ( fltSSliderArea     );
    fltReleaseSlider.setBounds ( fltADSRAreaReduced );
    
    // LFO Area
    auto lfoArea = bottomSectionArea;
    
    juce::Rectangle<int> lfoAreaReduced = lfoArea.reduced( sectionSpacerSize );
    
    lfoSectionInner.setBounds( lfoAreaReduced.getX(), lfoAreaReduced.getY(), lfoAreaReduced.getWidth(), lfoAreaReduced.getHeight() );
    
    juce::Rectangle<int> lfoVertSliderArea    = lfoAreaReduced.removeFromRight      ( (int)( lfoAreaReduced.getWidth() * 0.33f ) );
    juce::Rectangle<int> lfoVertLabelFootArea = lfoVertSliderArea.removeFromTop     ( filtLabelheight );
    juce::Rectangle<int> lfoFrqLabelArea      = lfoVertLabelFootArea.removeFromLeft ( (int)( lfoVertLabelFootArea.getWidth() * 0.5f ) );
    
    lfoFreqLabel.setBounds   ( lfoFrqLabelArea      );
    lfoAmountLabel.setBounds ( lfoVertLabelFootArea );
    
    juce::Rectangle<int> lfoFreqSliderArea = lfoVertSliderArea.removeFromLeft( (int)( lfoVertSliderArea.getWidth() * 0.5f ) );
    
    lfoFreqSlider.setBounds   ( lfoFreqSliderArea );
    lfoAmountSlider.setBounds ( lfoVertSliderArea );
    
    juce::Rectangle<int> lfoShapeArea       = lfoAreaReduced.removeFromBottom( filtLabelheight );
    juce::Rectangle<int> lfoShapeSliderArea = lfoAreaReduced.removeFromBottom( filtLabelheight );
    
    lfoShapeLabel.setBounds  ( lfoShapeArea       );
    lfoShapeSlider.setBounds ( lfoShapeSliderArea );
    lfoVisual.setBounds      ( lfoAreaReduced     );
    
    
}

void DirtyLittleBassSynthAudioProcessorEditor::sliderSetup(juce::Slider& sliderInstance, juce::Slider::SliderStyle style,
                                                           juce::Colour& sliderFillColor, juce::Colour& sliderThumbColor,
                                                           bool showTextBox)
{
    sliderInstance.setSliderStyle(style);
    
    // If slider has a textbox, draw it, otherwise, don't
    if (showTextBox)
    {
        sliderInstance.setTextBoxStyle ( juce::Slider::TextBoxBelow, false, 40, 20 );
        sliderInstance.setColour       ( juce::Slider::textBoxOutlineColourId, juce::Colour( (juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0 ) );
        sliderInstance.setColour       ( juce::Slider::textBoxTextColourId, textColor );
    }
    else
    {
        sliderInstance.setTextBoxStyle( juce::Slider::NoTextBox, false, 0, 0 );
    }
    
    // juce::Slider types
    if (sliderInstance.getSliderStyle() == juce::Slider::SliderStyle::LinearHorizontal)
    {
        sliderInstance.setColour( juce::Slider::trackColourId, sliderFillColor );
    }
    else if (sliderInstance.getSliderStyle() == juce::Slider::SliderStyle::LinearVertical)
    {
        sliderInstance.setColour( juce::Slider::trackColourId, sliderFillColor );
    }
    else if (sliderInstance.getSliderStyle() == juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag)
    {
        sliderInstance.setColour( juce::Slider::rotarySliderFillColourId, sliderFillColor );
    }
    
    sliderInstance.setColour( juce::Slider::thumbColourId, sliderThumbColor );
    
    addAndMakeVisible(sliderInstance);
}


void DirtyLittleBassSynthAudioProcessorEditor::sliderLabelSetup(juce::Label& labelInstance, juce::String labelText, juce::Colour& c, float fontSize)
{
    labelInstance.setFont              ( juce::Font("helvetica", fontSize, 1) );
    labelInstance.setText              ( labelText, juce::dontSendNotification );
    labelInstance.setJustificationType ( juce::Justification::centred );
    labelInstance.setColour            ( juce::Label::textColourId, c );
    
    addAndMakeVisible(labelInstance);
}

void DirtyLittleBassSynthAudioProcessorEditor::comboBoxSetup(juce::ComboBox& boxInstance, juce::StringArray boxItems)
{
    boxInstance.addItemList          ( boxItems, 1 );
    boxInstance.setJustificationType ( juce::Justification::centred );
    boxInstance.setSelectedItemIndex ( 0 );
    addAndMakeVisible                ( boxInstance );
}
