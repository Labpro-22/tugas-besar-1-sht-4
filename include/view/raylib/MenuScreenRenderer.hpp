#pragma once

#include "controller/GUIController/GUIGameController.hpp"
#include "view/raylib/UiToolkit.hpp"

namespace view::raylibgui {
class MenuScreenRenderer {
public:
    void draw(GUIGameController& controller, const UiToolkit& toolkit) const;

private:
    void drawMainMenu(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawNewGame(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawLoadGame(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawTutorial(GUIGameController& controller, const UiToolkit& toolkit) const;
    void drawStepper(
        const UiToolkit& toolkit,
        const std::string& label,
        int& value,
        int minValue,
        int maxValue,
        int step,
        Rectangle rect
    ) const;
};
}
