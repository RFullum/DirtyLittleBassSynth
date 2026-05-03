/*
  ==============================================================================

    DryWetSlider.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//============================================================

/// Slider variant for dry/wet (mix) controls. Adds two click-to-set gestures on
/// top of the standard juce::Slider behaviour:
///   • Shift-click       → midpoint of the slider's range (typically 50% mix)
///   • Cmd/Ctrl-click    → maximum of the slider's range  (typically 100% mix)
///
/// The OS-aware modifier comes from juce::ModifierKeys::commandModifier — Cmd
/// on macOS, Ctrl on Windows/Linux — matching each platform's convention for
/// "alternate-action click."
///
/// Works alongside juce::Slider::setDoubleClickReturnValue(true, 0.0) which is
/// used elsewhere to drive double-click → 0% behaviour.
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
