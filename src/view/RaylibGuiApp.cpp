#include "view/RaylibGuiApp.hpp"

#include "raylib.h"

using namespace view::raylibgui;

namespace {
constexpr const char* kGuiFontPath = "assets/fonts/gui.ttf";
constexpr int kGuiFontBaseSize = 64;
}

RaylibGuiApp::RaylibGuiApp() {}

void RaylibGuiApp::run() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1440, 900, "NIMONSPOLI");
    SetTargetFPS(60);
    toolkit_.loadFont(kGuiFontPath, kGuiFontBaseSize);

    while (!WindowShouldClose() && !controller_.state().isQuitRequested()) {
        const float deltaTime = GetFrameTime();
        controller_.tick(deltaTime);
        controller_.handleGlobalShortcuts();

        BeginDrawing();
        ClearBackground(toolkit_.theme().getPaperSoft());
        toolkit_.drawBackground(controller_.state().getTime());

        if (controller_.state().getScreen() == Screen::Gameplay) {
            gameplayRenderer_.draw(controller_, toolkit_);
        } else {
            menuRenderer_.draw(controller_, toolkit_);
        }

        overlayRenderer_.draw(controller_, toolkit_);
        gameplayRenderer_.drawToasts(controller_, toolkit_);

        EndDrawing();
    }

    toolkit_.unloadFont();
    CloseWindow();
}
