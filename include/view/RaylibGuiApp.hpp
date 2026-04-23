#pragma once

#include "controller/GUIController/GUIGameController.hpp"
#include "view/raylib/GameplayScreenRenderer.hpp"
#include "view/raylib/MenuScreenRenderer.hpp"
#include "view/raylib/OverlayRenderer.hpp"
#include "view/raylib/UiToolkit.hpp"

class RaylibGuiApp {
public:
    RaylibGuiApp();
    void run();

private:
    view::raylibgui::UiToolkit toolkit_;
    GUIGameController controller_;
    view::raylibgui::MenuScreenRenderer menuRenderer_;
    view::raylibgui::GameplayScreenRenderer gameplayRenderer_;
    view::raylibgui::OverlayRenderer overlayRenderer_;
};
