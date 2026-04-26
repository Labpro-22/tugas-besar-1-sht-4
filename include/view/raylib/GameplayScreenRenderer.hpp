#pragma once

#include "controller/GUIController/GUIGameController.hpp"
#include "view/raylib/UiToolkit.hpp"

namespace view::raylibgui {
class GameplayScreenRenderer {
public:
    void draw(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawToasts(const GUIGameController& controller, const UiToolkit& toolkit) const;

private:
    static int effectiveMoneyFor(const PlayerInfo& player, int amount);

    void drawCurrentPlayerPanel(
        const GUIGameController& controller,
        const UiToolkit& toolkit,
        Rectangle rect
    ) const;

    void drawBoardPanel(
        GUIGameController& controller,
        const UiToolkit& toolkit,
        Rectangle rect,
        bool interactive
    ) const;

    void drawActionPanel(
        GUIGameController& controller,
        const UiToolkit& toolkit,
        Rectangle rect,
        bool interactive
    ) const;

    void drawInteractionLab(
        GUIGameController& controller,
        const UiToolkit& toolkit,
        Rectangle rect,
        bool interactive
    ) const;

    void drawLogsPreview(
        const GUIGameController& controller,
        const UiToolkit& toolkit,
        Rectangle rect
    ) const;
};
}
