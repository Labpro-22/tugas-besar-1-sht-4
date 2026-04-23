#pragma once

#include "view/raylib/GuiTypes.hpp"

class GUIGameController;

class GUICommandController {
public:
    explicit GUICommandController(GUIGameController& controller);

    void tick(float deltaTime);
    void handleGlobalShortcuts();

    void openOverlay(view::raylibgui::OverlayType type);
    void closeOverlay();
    bool isOverlayOpen() const;

    void openPortfolio();
    void openSave();
    void openLogs();
    void openSetDice();
    void openHelp();
    void openForceDrop();
    void openGameOver();

    void startTurn();
    void rollDice();
    void applyManualDice();
    void endTurn();
    bool canSaveNow() const;
    void adjustManualDie(int dieIndex, int delta);
    void saveSession();

private:
    GUIGameController& controller_;
};
