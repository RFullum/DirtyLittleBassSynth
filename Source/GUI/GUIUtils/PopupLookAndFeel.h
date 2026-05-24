/*
  ==============================================================================

    PopupLookAndFeel.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

//==============================================================================

class PopupLookAndFeel
    : public juce::LookAndFeel_V4
{
public:
    explicit PopupLookAndFeel(const Palette::Theme &themeRef);
    ~PopupLookAndFeel() override = default;

    void  drawPopupMenuBackground(juce::Graphics &g, int width, int height) override;

    void  drawPopupMenuItem(juce::Graphics               &g
                            , const juce::Rectangle<int> &area
                            , bool                        isSeparator
                            , bool                        isActive
                            , bool                        isHighlighted
                            , bool                        isTicked
                            , bool                        hasSubMenu
                            , const juce::String         &text
                            , const juce::String         &shortcutKeyText
                            , const juce::Drawable       *icon
                            , const juce::Colour         *textColour) override;

    juce::Font getPopupMenuFont() override;

    void  getIdealPopupMenuItemSize(const juce::String &text
                                    , bool              isSeparator
                                    , int               standardMenuItemHeight
                                    , int              &idealWidth
                                    , int              &idealHeight) override;

    void                 drawTooltip      (juce::Graphics &g, const juce::String &text, int width, int height) override;
    juce::Rectangle<int> getTooltipBounds (const juce::String &tipText, juce::Point<int> screenPos, juce::Rectangle<int> parentArea) override;

    void drawAlertBox (juce::Graphics               &g
                       , juce::AlertWindow          &alert
                       , const juce::Rectangle<int> &textArea
                       , juce::TextLayout           &textLayout) override;
    juce::Font getAlertWindowFont       () override;
    juce::Font getAlertWindowTitleFont  () override;
    juce::Font getAlertWindowMessageFont() override;

private:
    const Palette::Theme &theme;

    static constexpr float cornerRadius     = 4.0f;
    static constexpr int   itemPaddingX     = 12;
    static constexpr int   itemHeight       = 24;
    static constexpr int   separatorHeight  = 9;   // 1px line centred in this row

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PopupLookAndFeel)
};
