/*
  ==============================================================================

    SegmentedControl.cpp

  ==============================================================================
*/

#include "SegmentedControl.h"

//==============================================================================

SegmentedControl::SegmentedControl()
: activeFill  (juce::Colours::white)
, outlineColor(juce::Colours::grey)
, textColor   (juce::Colours::white)
{
    setOpaque(false);
}

void SegmentedControl::paint(juce::Graphics &g)
{
    static constexpr float corner = 2.0f;

    g.setFont(juce::Font(juce::FontOptions("Helvetica", fontSize, juce::Font::bold))
                 .withExtraKerningFactor(0.10f));

    for (int i = 0; i < (int)segmentBounds.size() && i < segments.size(); ++i)
    {
        auto rect = segmentBounds[(size_t)i].toFloat();
        const bool active = (i == selectedIndex);

        if (active)
        {
            g.setColour(activeFill.withAlpha(0.15f));
            g.fillRoundedRectangle(rect, corner);
            g.setColour(activeFill);
            g.drawRoundedRectangle(rect.reduced(0.5f), corner, 1.0f);
            g.setColour(activeFill);
        }
        else
        {
            g.setColour(outlineColor);
            g.drawRoundedRectangle(rect.reduced(0.5f), corner, 1.0f);
            g.setColour(textColor);
        }

        g.drawText(segments[i], rect.toNearestInt(), juce::Justification::centred);
    }
}

void SegmentedControl::resized()
{
    segmentBounds.clear();

    if (segments.isEmpty())
        return;

    static constexpr int segmentGap = 6;

    auto      bounds = getLocalBounds();
    const int n      = segments.size();
    const int totalW = bounds.getWidth() - segmentGap * (n - 1);
    const int w      = totalW / n;

    for (int i = 0; i < n; ++i)
    {
        const bool last = (i == n - 1);
        auto cell = last ? bounds : bounds.removeFromLeft(w);
        segmentBounds.push_back(cell);

        if (!last)
            bounds.removeFromLeft(segmentGap);
    }
}

void SegmentedControl::mouseDown(const juce::MouseEvent &e)
{
    for (int i = 0; i < (int)segmentBounds.size(); ++i)
    {
        if (segmentBounds[(size_t)i].contains(e.getPosition()))
        {
            if (parameter != nullptr && attachment != nullptr)
                attachment->setValueAsCompleteGesture((float)i);

            return;
        }
    }
}

void SegmentedControl::Setup(juce::AudioProcessorValueTreeState &apvts
                             , const juce::String               &parameterID
                             , const juce::StringArray          &items
                             , juce::Colour                      activeFillIn
                             , juce::Colour                      outlineColorIn
                             , juce::Colour                      textColorIn
                             , float                             fontSizeIn)
{
    segments     = items;
    activeFill   = activeFillIn;
    outlineColor = outlineColorIn;
    textColor    = textColorIn;
    fontSize     = fontSizeIn;

    parameter = apvts.getParameter(parameterID);

    // Tag for MidiLearnOverlay arm-on-click (matches AttachSlider/AttachButton).
    getProperties().set("paramID", parameterID);

    if (parameter != nullptr)
    {
        attachment = std::make_unique<juce::ParameterAttachment>(
            *parameter,
            [this](float newValue)
            {
                if (auto *choice = dynamic_cast<juce::AudioParameterChoice *>(parameter))
                    selectedIndex = choice->getIndex();
                else
                    selectedIndex = (int)newValue;

                repaint();
            });

        attachment->sendInitialUpdate();
    }
}
