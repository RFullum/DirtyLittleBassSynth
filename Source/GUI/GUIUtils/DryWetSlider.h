/*
  ==============================================================================

    DryWetSlider.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

// Slider extensions for mix controls:
//   Shift-click    -> midpoint (50% mix)
//   Cmd/Ctrl-click -> maximum  (100% mix)
//   Double-click   -> 0% (via setDoubleClickReturnValue at the call site)
class DryWetSlider
    : public juce::Slider
{
public:
    DryWetSlider() = default;

    void mouseDown(const juce::MouseEvent &e) override
    {
        const bool shiftOnly = e.mods.isShiftDown()   && ! e.mods.isCommandDown();
        const bool cmdOnly   = e.mods.isCommandDown() && ! e.mods.isShiftDown();

        if (shiftOnly)
        {
            setValue((getMinimum() + getMaximum()) * 0.5, juce::sendNotificationSync);
            return;
        }

        if (cmdOnly)
        {
            setValue(getMaximum(), juce::sendNotificationSync);
            return;
        }

        juce::Slider::mouseDown(e);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DryWetSlider)
};
