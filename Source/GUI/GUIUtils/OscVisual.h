/*
  ==============================================================================

    OscVisual.h
    Created: 29 Dec 2020 12:01:58pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Wavetable.h"
#include "OscillatorParameterControls.h"

//==============================================================================

/// Visualizes the morph blend between three wavetable shapes for the main, sub, or
/// filter-LFO oscillators. Reads the morph parameter on the UI thread and computes
/// its own waveform; no audio-thread coupling.
class OscVisual
    : public juce::Component
{
public:
    OscVisual();
    ~OscVisual() override = default;

    /// One-time setup: which morph parameter to follow and whether the centre wavetable
    /// is the main-osc spike (false) or the sub/LFO square (true). Optionally pass a
    /// gain parameter (0..1) to scale the displayed wave amplitude — used for the sub
    /// visual so it reflects the Sub Gain knob. Call once after construction.
    void Init(std::atomic<float> *morphParamIn, bool useSquareIn, std::atomic<float> *gainParamIn = nullptr);

    /// Reads the morph parameter, rebuilds the displayed path, repaints. Call from the
    /// editor's timer.
    void Update();

    /// Sets the colours of the wave line and background gradient.
    void SetColors(juce::Colour line, juce::Colour background, juce::Colour fade);

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void RebuildPath();

    static constexpr int waveTableSize = 1024;

    std::atomic<float> *morphParam = nullptr;
    std::atomic<float> *gainParam  = nullptr;     // optional; scales displayed amplitude (0..1)
    bool                useSquare  = false;

    Wavetable       wtSine;
    SpikeWavetable  wtSpike;
    SquareWavetable wtSquare;
    SawWavetable    wtSaw;

    OscParamControl morphControl;

    juce::Colour lineColor;
    juce::Colour bgColor;
    juce::Colour fadeColor;

    juce::Rectangle<float> visualBox;

    juce::Path oscShape;
    juce::Path oscArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscVisual)
};
