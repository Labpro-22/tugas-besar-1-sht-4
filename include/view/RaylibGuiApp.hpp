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
    static constexpr const char* GuiFontPath = "assets/fonts/gui.ttf";
    static constexpr int GuiFontBaseSize = 64;

    view::raylibgui::UiToolkit toolkit_;
    GUIGameController controller_;
    view::raylibgui::MenuScreenRenderer menuRenderer_;
    view::raylibgui::GameplayScreenRenderer gameplayRenderer_;
    view::raylibgui::OverlayRenderer overlayRenderer_;
};
