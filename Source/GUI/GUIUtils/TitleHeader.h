/*
  ==============================================================================

    TitleHeader.h
    Created: 12 Jan 2021 11:10:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

//==============================================================================

/// Top-of-window header strip: plugin name + tagline on the left,
/// preset navigation placeholder + brand text on the right.
class TitleHeader
    : public juce::Component
{
public:
    TitleHeader();
    ~TitleHeader() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void SetTheme(const Palette::Theme &t);

private:
    const Palette::Theme *theme = nullptr;

    juce::Rectangle<int> pluginNameRect;
    juce::Rectangle<int> taglineRect;
    juce::Rectangle<int> initPatchRect;
    juce::Rectangle<int> prevBtnRect;
    juce::Rectangle<int> nextBtnRect;
    juce::Rectangle<int> brandingRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleHeader)
};

//==============================================================================

/// Bottom-of-window footer strip: version on the left, URL on the right.
class TitleFooter
    : public juce::Component
{
public:
    TitleFooter();
    ~TitleFooter() override = default;

    void paint(juce::Graphics &) override;
    void resized() override;

    void setTheme(const Palette::Theme &t);

private:
    const Palette::Theme *theme = nullptr;

    juce::Rectangle<int> versionRect;
    juce::Rectangle<int> urlRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleFooter)
};
