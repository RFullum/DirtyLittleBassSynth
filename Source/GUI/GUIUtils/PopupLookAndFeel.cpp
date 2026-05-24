/*
  ==============================================================================

    PopupLookAndFeel.cpp

  ==============================================================================
*/

#include "PopupLookAndFeel.h"

//==============================================================================

PopupLookAndFeel::PopupLookAndFeel(const Palette::Theme &themeRef)
: theme(themeRef)
{
    // Alpha < 1.0 flips isOpaque() to false so JUCE's MenuWindow skips its
    // corner-clobbering fillAll. Stay under ~0.998f (byte 255 would round opaque).
    setColour(juce::PopupMenu::backgroundColourId,            theme.background.withAlpha(0.99f));
    setColour(juce::PopupMenu::textColourId,                  theme.textPrimary);
    setColour(juce::PopupMenu::headerTextColourId,            theme.textSecondary);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, theme.primaryAccent.withAlpha(0.20f));
    setColour(juce::PopupMenu::highlightedTextColourId,       theme.textPrimary);

    setColour(juce::AlertWindow::backgroundColourId, theme.background);
    setColour(juce::AlertWindow::textColourId,       theme.textPrimary);
    setColour(juce::AlertWindow::outlineColourId,    juce::Colours::transparentBlack);

    setColour(juce::TextEditor::backgroundColourId,     theme.structure);
    setColour(juce::TextEditor::textColourId,           theme.textPrimary);
    setColour(juce::TextEditor::outlineColourId,        juce::Colours::transparentBlack);
    setColour(juce::TextEditor::focusedOutlineColourId, theme.primaryAccent);
    setColour(juce::TextEditor::highlightColourId,      theme.primaryAccent.withAlpha(0.35f));
    setColour(juce::TextEditor::highlightedTextColourId, theme.textPrimary);
    setColour(juce::TextEditor::shadowColourId,         juce::Colours::transparentBlack);

    setColour(juce::TextButton::buttonColourId,    theme.structure);
    setColour(juce::TextButton::buttonOnColourId,  theme.primaryAccent.withAlpha(0.25f));
    setColour(juce::TextButton::textColourOffId,   theme.textPrimary);
    setColour(juce::TextButton::textColourOnId,    theme.textPrimary);
}

void PopupLookAndFeel::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
{
    const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height);

    g.setColour(theme.background);
    g.fillRoundedRectangle(bounds, cornerRadius);
}

void PopupLookAndFeel::drawPopupMenuItem(juce::Graphics               &g
                                         , const juce::Rectangle<int> &area
                                         , bool                        isSeparator
                                         , bool                        isActive
                                         , bool                        isHighlighted
                                         , bool                        isTicked
                                         , bool                        hasSubMenu
                                         , const juce::String         &text
                                         , const juce::String         &shortcutKeyText
                                         , const juce::Drawable       *icon
                                         , const juce::Colour         *textColourOverride)
{
    if (isSeparator)
    {
        auto line = area.toFloat().reduced(itemPaddingX * 0.5f, 0.0f);
        line.setY(area.getCentreY() - 0.5f);
        line.setHeight(1.0f);

        g.setColour(theme.structure);
        g.fillRect(line);
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour(findColour(juce::PopupMenu::highlightedBackgroundColourId));
        g.fillRoundedRectangle(area.toFloat().reduced(3.0f, 1.0f), cornerRadius * 0.75f);
    }

    const auto baseText = textColourOverride != nullptr
                              ? *textColourOverride
                              : findColour(isHighlighted
                                               ? juce::PopupMenu::highlightedTextColourId
                                               : juce::PopupMenu::textColourId);

    const auto textColour = isActive ? baseText : baseText.withMultipliedAlpha(0.4f);

    auto contentArea = area.reduced(itemPaddingX, 0);

    if (isTicked)
    {
        const int  tickSize = juce::jmin(contentArea.getHeight() - 6, 12);
        const auto tickArea = contentArea.removeFromLeft(tickSize + 6)
                                         .withSizeKeepingCentre(tickSize, tickSize);

        g.setColour(theme.primaryAccent);
        juce::Path tick;
        tick.startNewSubPath((float)tickArea.getX(),                                (float)tickArea.getCentreY());
        tick.lineTo         ((float)tickArea.getX() + tickArea.getWidth() * 0.4f,   (float)tickArea.getBottom());
        tick.lineTo         ((float)tickArea.getRight(),                            (float)tickArea.getY());
        g.strokePath(tick, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    else if (icon != nullptr)
    {
        const auto iconArea = contentArea.removeFromLeft(contentArea.getHeight()).reduced(3);
        icon->drawWithin(g, iconArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);
    }

    if (hasSubMenu)
    {
        const int  arrowSize = juce::jmin(contentArea.getHeight() / 3, 6);
        const auto arrowArea = contentArea.removeFromRight(arrowSize + 6)
                                          .withSizeKeepingCentre(arrowSize, arrowSize);

        juce::Path arrow;
        arrow.addTriangle((float)arrowArea.getX(),       (float)arrowArea.getY()
                          , (float)arrowArea.getX(),     (float)arrowArea.getBottom()
                          , (float)arrowArea.getRight(), (float)arrowArea.getCentreY());

        g.setColour(textColour.withAlpha(0.7f));
        g.fillPath(arrow);
    }

    if (shortcutKeyText.isNotEmpty())
    {
        // Take only what the shortcut needs. JUCE pre-concatenates it into
        // getIdealPopupMenuItemSize's text, so the row is already wide enough.
        auto       shortcutFont  = getPopupMenuFont().withHeight(getPopupMenuFont().getHeight() * 0.85f);
        const int  shortcutWidth = juce::GlyphArrangement::getStringWidthInt(shortcutFont, shortcutKeyText) + 8;
        auto       shortcutArea  = contentArea.removeFromRight(shortcutWidth);

        g.setFont(shortcutFont);
        g.setColour(textColour.withMultipliedAlpha(0.55f));
        g.drawText(shortcutKeyText, shortcutArea, juce::Justification::centredRight, true);
    }

    g.setFont(getPopupMenuFont());
    g.setColour(textColour);
    g.drawText(text, contentArea, juce::Justification::centredLeft, true);
}

juce::Font PopupLookAndFeel::getPopupMenuFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 13.0f, 0))
           .withExtraKerningFactor(0.04f);
}

void PopupLookAndFeel::drawTooltip(juce::Graphics &g, const juce::String &text, int width, int height)
{
    const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height);

    g.setColour(theme.background);
    g.fillRoundedRectangle(bounds, cornerRadius);

    juce::AttributedString s;
    s.append(text, getPopupMenuFont(), theme.textPrimary);
    s.setJustification(juce::Justification::centred);

    juce::TextLayout layout;
    layout.createLayout(s, (float) width - 12.0f);
    layout.draw(g, bounds.reduced(6.0f, 4.0f));
}

juce::Rectangle<int> PopupLookAndFeel::getTooltipBounds(const juce::String   &tipText
                                                        , juce::Point<int>     screenPos
                                                        , juce::Rectangle<int> parentArea)
{
    juce::AttributedString s;
    s.append(tipText, getPopupMenuFont());

    juce::TextLayout layout;
    layout.createLayout(s, 400.0f);

    const int w = (int) std::ceil(layout.getWidth())  + 16;
    const int h = (int) std::ceil(layout.getHeight()) + 10;

    return juce::Rectangle<int>(screenPos.x > parentArea.getCentreX() ? screenPos.x - (w + 12) : screenPos.x + 24,
                                screenPos.y > parentArea.getCentreY() ? screenPos.y - (h + 6)  : screenPos.y + 6,
                                w, h).constrainedWithin(parentArea);
}

void PopupLookAndFeel::getIdealPopupMenuItemSize(const juce::String &text
                                                 , bool              isSeparator
                                                 , int               standardMenuItemHeight
                                                 , int              &idealWidth
                                                 , int              &idealHeight)
{
    if (isSeparator)
    {
        idealWidth  = 50;
        idealHeight = separatorHeight;
        return;
    }

    auto font   = getPopupMenuFont();
    idealWidth  = juce::GlyphArrangement::getStringWidthInt(font, text) + itemPaddingX * 2 + 24;
    idealHeight = standardMenuItemHeight > 0 ? standardMenuItemHeight : itemHeight;
}

void PopupLookAndFeel::drawAlertBox(juce::Graphics              &g
                                    , juce::AlertWindow         &alert
                                    , const juce::Rectangle<int> &textArea
                                    , juce::TextLayout          &textLayout)
{
    const auto bounds = alert.getLocalBounds().toFloat();

    g.setColour(theme.background);
    g.fillRoundedRectangle(bounds, cornerRadius);

    juce::ignoreUnused(alert);

    // Recolour title runs in place: title font is taller than the body, so any
    // run above the body-font height is part of the title.
    const float bodyFontHeight = getAlertWindowMessageFont().getHeight();

    for (int line = 0; line < textLayout.getNumLines(); ++line)
        for (auto *run : textLayout.getLine(line).runs)
            if (run != nullptr && run->font.getHeight() > bodyFontHeight + 0.5f)
                run->colour = theme.orangeAccent;

    textLayout.draw(g, textArea.toFloat());
}

juce::Font PopupLookAndFeel::getAlertWindowFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 13.0f, 0));
}

juce::Font PopupLookAndFeel::getAlertWindowTitleFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 16.0f, juce::Font::bold))
           .withExtraKerningFactor(0.04f);
}

juce::Font PopupLookAndFeel::getAlertWindowMessageFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 13.0f, 0));
}
