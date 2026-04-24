#pragma once

#include "controller/GUIController/GUIControllerContext.hpp"

class GUICommandController {
public:
    explicit GUICommandController(GUIControllerContext& context);

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
    void finishTurnAfterDiceIfReady();
    bool canSaveNow() const;
    void adjustManualDie(int dieIndex, int delta);
    void saveSession();

private:
    GUIControllerContext& controller_;
};
