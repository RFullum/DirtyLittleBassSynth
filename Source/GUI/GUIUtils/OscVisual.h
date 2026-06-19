/*
  ==============================================================================

    OscVisual.h
    Created: 29 Dec 2020 12:01:58pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/Wavetable.h"

//==============================================================================

// Renders the morph blend between three wavetable shapes. UI-thread only;
// computes its own waveform from the morph parameter, no audio coupling.
class OscVisual
    : public juce::Component
{
public:
    OscVisual();
    ~OscVisual() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void Init(std::atomic<float> *morphParamIn, bool useSquareIn, std::atomic<float> *gainParamIn = nullptr);
    void Update();
    void SetColors(juce::Colour line, juce::Colour background, juce::Colour fade);
    
private:
    void RebuildPath();

    static constexpr int waveTableSize = 1024;

    std::atomic<float> *morphParam = nullptr;
    std::atomic<float> *gainParam  = nullptr;     // optional; scales display amplitude
    bool                useSquare  = false;

    FullumOscillators::Wavetable       wtSine;
    FullumOscillators::SpikeWavetable  wtSpike;
    FullumOscillators::SquareWavetable wtSquare;
    FullumOscillators::SawWavetable    wtSaw;

    juce::Colour lineColor;
    juce::Colour bgColor;
    juce::Colour fadeColor;

    juce::Rectangle<float> visualBox;

    juce::Path oscShape;
    juce::Path oscArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscVisual)
};
