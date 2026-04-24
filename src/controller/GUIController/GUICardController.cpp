#include "controller/GUIController/GUICardController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

using namespace view::raylibgui;

GUICardController::GUICardController(GUIGameController& controller)
    : controller_(controller) {}

void GUICardController::openCards() {
    controller_.openCards();
}

void GUICardController::openRandomCardDraw(int deckKey) {
    controller_.openRandomCardDraw(deckKey);
}

void GUICardController::useSelectedHandCard() {
    controller_.useSelectedHandCard();
}

void GUICardController::storeDrawnCard() {
    controller_.storeDrawnCard();
}

void GUICardController::applyDrawnCard() {
    controller_.applyDrawnCard();
}

void GUICardController::dropSelectedHandCard() {
    controller_.dropSelectedHandCard();
}

void GUICardController::useJailCard() {
    controller_.useJailCard();
}

bool GUICardController::currentPlayerNeedsForceDrop() const {
    const GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return false;
    }
    return game.getPlayers().at(game.getCurrentPlayer()).getHandCards().size() > 3U;
}

void GUICardController::maybeOpenForceDrop() {
    if (currentPlayerNeedsForceDrop()) {
        controller_.openForceDrop();
    }
}

CardInfo GUICardController::pickRandomCard(int deckKey) {
    (void) deckKey;
    return {};
}

std::vector<CardInfo> GUICardController::chanceDeck() const {
    return {};
}

std::vector<CardInfo> GUICardController::communityDeck() const {
    return {};
}
