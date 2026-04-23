#include "controller/GUIController/GUICommandController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

#include <algorithm>

using namespace view::raylibgui;

GUICommandController::GUICommandController(GUIGameController& controller)
    : controller_(controller) {}

void GUICommandController::tick(float deltaTime) {
    AppState& state = controller_.state();
    state.setTime(state.getTime() + deltaTime);
    if (state.getOverlay().getType() == OverlayType::None) {
        state.getOverlay().setAnim(0.0f);
        return;
    }

    state.getOverlay().setAnim(std::min(1.0f, state.getOverlay().getAnim() + deltaTime * 8.0f));
}

void GUICommandController::handleGlobalShortcuts() {}

void GUICommandController::openOverlay(OverlayType type) {
    AppState& state = controller_.state();
    state.getOverlay().setType(type);
    state.getOverlay().setAnim(0.0f);
    state.setActiveField(type == OverlayType::Save ? "save-name" : "");
}

void GUICommandController::closeOverlay() {
    AppState& state = controller_.state();
    state.setOverlay(OverlayState{});
    state.getActiveField().clear();
}

bool GUICommandController::isOverlayOpen() const {
    return controller_.state().getOverlay().getType() != OverlayType::None;
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
    AppState& state = controller_.state();
    state.getOverlay().setManualDieOne(std::max(1, state.getGame().getLastDieOne()));
    state.getOverlay().setManualDieTwo(std::max(1, state.getGame().getLastDieTwo()));
    openOverlay(OverlayType::SetDice);
}

void GUICommandController::openHelp() {
    openOverlay(OverlayType::Help);
}

void GUICommandController::openForceDrop() {
    controller_.state().getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::ForceDrop);
}

void GUICommandController::openGameOver() {
    openOverlay(OverlayType::GameOver);
}

void GUICommandController::startTurn() {
    controller_.state().getGame().setTurnStarted(true);
    controller_.state().getGame().setRolledThisTurn(false);
}

void GUICommandController::rollDice() {}

void GUICommandController::applyManualDice() {}

void GUICommandController::endTurn() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    game.setCurrentPlayer((game.getCurrentPlayer() + 1) % static_cast<int>(game.getPlayers().size()));
    game.setTurn(game.getTurn() + 1);
    game.setTurnStarted(false);
    game.setRolledThisTurn(false);
}

bool GUICommandController::canSaveNow() const {
    return !controller_.state().getGame().isRolledThisTurn();
}

void GUICommandController::adjustManualDie(int dieIndex, int delta) {
    OverlayState& overlay = controller_.state().getOverlay();
    const int die = dieIndex == 1 ? overlay.getManualDieOne() : overlay.getManualDieTwo();
    const int adjusted = std::max(1, std::min(6, die + delta));
    if (dieIndex == 1) {
        overlay.setManualDieOne(adjusted);
        return;
    }

    overlay.setManualDieTwo(adjusted);
}

void GUICommandController::saveSession() {}
