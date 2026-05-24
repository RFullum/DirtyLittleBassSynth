/*
  ==============================================================================

    PatchSelectionPopup.cpp

  ==============================================================================
*/

#include "PatchSelectionPopup.h"
#include "PatchManager.h"

namespace
{
    static constexpr int popupWidth      = 1300;
    static constexpr int popupHeight     = 500;   // 15 rows × ~30 px + padding + scrollbar
    static constexpr int popupPadding    = 16;
    static constexpr int itemsPerColumn  = 15;
    static constexpr int columnWidth     = 200;
    static constexpr int scrollBarSpace  = 12;

    //============================================================

    class PatchGridItem
        : public juce::Component
    {
    public:
        PatchGridItem(const juce::String      &nameIn
                      , juce::Colour           textColourIn
                      , juce::Colour           hoverColourIn
                      , bool                   tickedIn
                      , std::function<void()>  onClickIn)
        : name(nameIn)
        , textColour(textColourIn)
        , hoverColour(hoverColourIn)
        , ticked(tickedIn)
        , onClick(std::move(onClickIn))
        {
            setOpaque(false);
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }

        void paint(juce::Graphics &g) override
        {
            const auto bounds = getLocalBounds().toFloat();

            if (hovered)
            {
                g.setColour(hoverColour);
                g.fillRoundedRectangle(bounds.reduced(2.0f), 3.0f);
            }

            auto content = getLocalBounds().reduced(10, 0);

            // Tick column on the left for the currently-loaded patch.
            const int tickCol = 16;
            auto tickArea = content.removeFromLeft(tickCol);

            if (ticked)
            {
                const int tickSize = juce::jmin(tickArea.getHeight() - 12, 10);
                const auto t = tickArea.withSizeKeepingCentre(tickSize, tickSize);

                g.setColour(textColour);
                juce::Path tick;
                tick.startNewSubPath((float) t.getX(),                              (float) t.getCentreY());
                tick.lineTo         ((float) t.getX() + t.getWidth() * 0.4f,        (float) t.getBottom());
                tick.lineTo         ((float) t.getRight(),                          (float) t.getY());
                g.strokePath(tick, juce::PathStrokeType(1.4f,
                                                        juce::PathStrokeType::curved,
                                                        juce::PathStrokeType::rounded));
            }

            g.setColour(textColour);
            g.setFont(juce::Font(juce::FontOptions("Helvetica", 13.0f, juce::Font::bold))
                      .withExtraKerningFactor(0.04f));
            g.drawText(name, content, juce::Justification::centredLeft, true);
        }

        void mouseEnter(const juce::MouseEvent &) override { hovered = true;  repaint(); }
        void mouseExit (const juce::MouseEvent &) override { hovered = false; repaint(); }

        void mouseUp(const juce::MouseEvent &e) override
        {
            if (! e.mouseWasDraggedSinceMouseDown() && onClick != nullptr)
                onClick();
        }

    private:
        juce::String name;
        juce::Colour textColour;
        juce::Colour hoverColour;
        bool         ticked;
        bool         hovered = false;
        std::function<void()> onClick;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchGridItem)
    };

//============================================================

    class PatchGridContent
        : public juce::Component
    {
    public:
        PatchGridContent(const Palette::Theme                         &theme
                         , const std::vector<PatchManager::PatchInfo> &patches
                         , const juce::File                           &currentFile
                         , std::function<void(juce::File)>             onSelectIn
                         , int                                         viewportHeightIn)
        : viewportHeight(viewportHeightIn)
        , onSelect      (std::move(onSelectIn))
        {
            setOpaque(false);

            const int rowH = juce::jmax(28, viewportHeight / itemsPerColumn);
            itemHeight     = rowH;

            items.reserve(patches.size());

            for (const auto &info : patches)
            {
                const auto colour = info.source == PatchManager::Source::Factory
                                        ? theme.primaryAccent
                                        : theme.secondaryAccent;

                const auto hover = colour.withAlpha(0.18f);
                const bool tick  = (info.file == currentFile);
                const auto file  = info.file;

                auto item = std::make_unique<PatchGridItem>(info.name
                                                            , colour
                                                            , hover
                                                            , tick
                                                            , [this, file]()
                                                              {
                                                                  if (onSelect)
                                                                      onSelect(file);
                                                              });
                addAndMakeVisible(item.get());
                items.push_back(std::move(item));
            }

            const int numCols = std::max(1, (int) std::ceil((double) items.size() / itemsPerColumn));
            setSize(numCols * columnWidth, itemHeight * itemsPerColumn);
        }

        void resized() override
        {
            for (size_t i = 0; i < items.size(); ++i)
            {
                const int col = (int) (i / itemsPerColumn);
                const int row = (int) (i % itemsPerColumn);
                items[i]->setBounds(col * columnWidth
                                    , row * itemHeight
                                    , columnWidth
                                    , itemHeight);
            }
        }

    private:
        int viewportHeight = 0;
        int itemHeight     = 0;

        std::vector<std::unique_ptr<PatchGridItem>> items;
        std::function<void(juce::File)>             onSelect;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchGridContent)
    };

//============================================================

    class PatchSelectionPopupComponent
        : public juce::Component
    {
    public:
        PatchSelectionPopupComponent(const GuiResources                           &resourcesIn
                                     , const std::vector<PatchManager::PatchInfo> &patches
                                     , const juce::File                           &currentFile)
        : resources(resourcesIn)
        {
            setOpaque       (false);
            setWantsKeyboardFocus(true);
            setInterceptsMouseClicks(true, true);

            const int viewportHeight = popupHeight - popupPadding * 2 - scrollBarSpace;

            auto contentOwned = std::make_unique<PatchGridContent>(resourcesIn.theme
                                                                   , patches
                                                                   , currentFile
                                                                   , [this](juce::File f)
                                                                     {
                                                                         chosenFile = f;
                                                                         exitModalState(1);
                                                                     }
                                                                   , viewportHeight);

            viewport.setViewedComponent(contentOwned.release(), true);
            viewport.setScrollBarsShown(false, true);
            viewport.setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::nonHover);
            addAndMakeVisible(viewport);
        }

        void paint(juce::Graphics &g) override
        {
            constexpr float cornerRadius = 6.0f;
            constexpr float borderWidth  = 1.5f;

            const auto bounds = getLocalBounds().toFloat();

            g.setColour(resources.theme.background);
            g.fillRoundedRectangle(bounds, cornerRadius);

            g.setColour(resources.theme.primaryAccent.withAlpha(0.5f));
            g.drawRoundedRectangle(bounds.reduced(borderWidth * 0.5f), cornerRadius, borderWidth);
        }

        void resized() override
        {
            viewport.setBounds(getLocalBounds().reduced(popupPadding));
        }

        bool keyPressed(const juce::KeyPress &key) override
        {
            if (key == juce::KeyPress::escapeKey)
            {
                exitModalState(0);
                return true;
            }

            return false;
        }

        void inputAttemptWhenModal() override
        {
            exitModalState(0);
        }

        const juce::File &GetChosenFile() const noexcept { return chosenFile; }

    private:
        const GuiResources &resources;
        juce::Viewport      viewport;
        juce::File          chosenFile;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchSelectionPopupComponent)
    };
}

//============================================================

PatchSelectionPopup::PatchSelectionPopup(GuiResources &res)
: resources(res)
{}

void PatchSelectionPopup::Show(juce::Component *targetComponent)
{
    if (resources.patchManager == nullptr || targetComponent == nullptr)
        return;

    auto *pm = resources.patchManager;
    pm->RefreshPatchList();

    auto *editor = targetComponent->getTopLevelComponent();
    if (editor == nullptr)
        return;

    // shared_ptr keeps the component alive across the lifetime
    auto popup = std::make_shared<PatchSelectionPopupComponent>(resources
                                                                , pm->GetPatchList()
                                                                , pm->GetCurrentPatchFile());

    editor->addAndMakeVisible(popup.get());

    const auto editorBounds = editor->getLocalBounds();
    const int  w            = juce::jmin(popupWidth,  editorBounds.getWidth());
    const int  h            = juce::jmin(popupHeight, editorBounds.getHeight());
    popup->setBounds(juce::Rectangle<int>(0, 0, w, h).withCentre(editorBounds.getCentre()));

    popup->grabKeyboardFocus();

    popup->enterModalState(true
                           , juce::ModalCallbackFunction::create(
                                 [popup, pm](int /*result*/)
                                 {
                                     const auto chosen = popup->GetChosenFile();

                                     if (auto *parent = popup->getParentComponent())
                                         parent->removeChildComponent(popup.get());

                                     if (chosen.existsAsFile())
                                         pm->LoadPatch(chosen);
                                 })
                           , false);
}
