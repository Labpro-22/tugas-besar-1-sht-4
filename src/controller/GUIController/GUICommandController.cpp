#include "controller/GUIController/GUICommandController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

using namespace view::raylibgui;

GUICommandController::GUICommandController(GUIGameController& controller)
    : controller_(controller) {}

void GUICommandController::tick(float deltaTime) {
    controller_.tick(deltaTime);
}

void GUICommandController::handleGlobalShortcuts() {
    controller_.handleGlobalShortcuts();
}

void GUICommandController::openOverlay(OverlayType type) {
    controller_.openOverlay(type);
}

void GUICommandController::closeOverlay() {
    controller_.closeOverlay();
}

bool GUICommandController::isOverlayOpen() const {
    return controller_.isOverlayOpen();
}

void GUICommandController::openPortfolio() {
    controller_.openPortfolio();
}

void GUICommandController::openSave() {
    controller_.openSave();
}

void GUICommandController::openLogs() {
    controller_.openLogs();
}

void GUICommandController::openSetDice() {
    controller_.openSetDice();
}

void GUICommandController::openHelp() {
    controller_.openHelp();
}

void GUICommandController::openForceDrop() {
    controller_.openForceDrop();
}

void GUICommandController::openGameOver() {
    controller_.openGameOver();
}

void GUICommandController::startTurn() {
    controller_.startTurn();
}

void GUICommandController::rollDice() {
    controller_.rollDice();
}

void GUICommandController::applyManualDice() {
    controller_.applyManualDice();
}

void GUICommandController::endTurn() {
    controller_.endTurn();
}

bool GUICommandController::canSaveNow() const {
    return controller_.canSaveNow();
}

void GUICommandController::adjustManualDie(int dieIndex, int delta) {
    controller_.adjustManualDie(dieIndex, delta);
}

void GUICommandController::saveSession() {
    controller_.saveSession();
}
