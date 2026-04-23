#include "controller/GUIController/GUICommandController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

#include <algorithm>

using namespace view::raylibgui;

GUICommandController::GUICommandController(GUIGameController& controller)
    : controller_(controller) {}

void GUICommandController::tick(float deltaTime) {
    controller_.state().time += deltaTime;
    if (controller_.state().overlay.type == OverlayType::None) {
        controller_.state().overlay.anim = 0.0f;
        return;
    }

    controller_.state().overlay.anim = std::min(1.0f, controller_.state().overlay.anim + deltaTime * 8.0f);
}

void GUICommandController::handleGlobalShortcuts() {}

void GUICommandController::openOverlay(OverlayType type) {
    controller_.state().overlay.type = type;
    controller_.state().overlay.anim = 0.0f;
    controller_.state().activeField = type == OverlayType::Save ? "save-name" : "";
}

void GUICommandController::closeOverlay() {
    controller_.state().overlay = OverlayState{};
    controller_.state().activeField.clear();
}

bool GUICommandController::isOverlayOpen() const {
    return controller_.state().overlay.type != OverlayType::None;
}

void GUICommandController::openPortfolio() {
    openOverlay(OverlayType::Portfolio);
}

void GUICommandController::openSave() {
    openOverlay(OverlayType::Save);
}

void GUICommandController::openLogs() {
    openOverlay(OverlayType::Logs);
}

void GUICommandController::openSetDice() {
    controller_.state().overlay.manualDieOne = std::max(1, controller_.state().game.lastDieOne);
    controller_.state().overlay.manualDieTwo = std::max(1, controller_.state().game.lastDieTwo);
    openOverlay(OverlayType::SetDice);
}

void GUICommandController::openHelp() {
    openOverlay(OverlayType::Help);
}

void GUICommandController::openForceDrop() {
    controller_.state().overlay.selectedIndex = 0;
    openOverlay(OverlayType::ForceDrop);
}

void GUICommandController::openGameOver() {
    openOverlay(OverlayType::GameOver);
}

void GUICommandController::startTurn() {
    controller_.state().game.turnStarted = true;
    controller_.state().game.rolledThisTurn = false;
}

void GUICommandController::rollDice() {}

void GUICommandController::applyManualDice() {}

void GUICommandController::endTurn() {
    if (controller_.state().game.players.empty()) {
        return;
    }

    controller_.state().game.currentPlayer =
        (controller_.state().game.currentPlayer + 1) % static_cast<int>(controller_.state().game.players.size());
    controller_.state().game.turn += 1;
    controller_.state().game.turnStarted = false;
    controller_.state().game.rolledThisTurn = false;
}

bool GUICommandController::canSaveNow() const {
    return !controller_.state().game.rolledThisTurn;
}

void GUICommandController::adjustManualDie(int dieIndex, int delta) {
    int& die = dieIndex == 1 ? controller_.state().overlay.manualDieOne : controller_.state().overlay.manualDieTwo;
    die = std::max(1, std::min(6, die + delta));
}

void GUICommandController::saveSession() {}
