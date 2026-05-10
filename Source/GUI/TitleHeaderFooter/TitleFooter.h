/*
  ==============================================================================

    TitleFooter.h
    Created: 10 May 2026 11:35:13am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

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
