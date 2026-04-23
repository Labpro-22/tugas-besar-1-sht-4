#include "controller/GUIController/GUICardController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

using namespace view::raylibgui;

GUICardController::GUICardController(GUIGameController& controller)
    : controller_(controller) {}

void GUICardController::openCards() {
    controller_.state().overlay.selectedIndex = 0;
    controller_.openOverlay(OverlayType::Cards);
}

void GUICardController::openRandomCardDraw(int deckKey) {
    controller_.state().overlay.deckKey = deckKey;
    controller_.state().overlay.card = pickRandomCard(deckKey);
    controller_.openOverlay(OverlayType::CardDraw);
}

void GUICardController::useSelectedHandCard() {}

void GUICardController::storeDrawnCard() {}

void GUICardController::applyDrawnCard() {}

void GUICardController::dropSelectedHandCard() {}

void GUICardController::useJailCard() {}

bool GUICardController::currentPlayerNeedsForceDrop() const {
    if (controller_.state().game.players.empty()) {
        return false;
    }

    const PlayerInfo& player = controller_.state().game.players.at(controller_.state().game.currentPlayer);
    return player.handCards.size() > 3U;
}

void GUICardController::maybeOpenForceDrop() {
    if (!currentPlayerNeedsForceDrop()) {
        return;
    }

    controller_.state().overlay.selectedIndex = 0;
    controller_.openOverlay(OverlayType::ForceDrop);
}

CardInfo GUICardController::pickRandomCard(int deckKey) {
    const std::vector<CardInfo> cards = deckKey == kCommunityDeckKey ? communityDeck() : chanceDeck();
    if (cards.empty()) {
        return {};
    }

    std::uniform_int_distribution<int> distribution(0, static_cast<int>(cards.size()) - 1);
    return cards.at(distribution(controller_.state().rng));
}

std::vector<CardInfo> GUICardController::chanceDeck() const {
    return {
        {"Teleport", "Pindah ke tile terpilih.", CardKind::Hand, CardEffect::TeleportAnywhere, 0, SKYBLUE},
        {"Go To Jail", "Masuk jail.", CardKind::Instant, CardEffect::GoToJail, 0, RED},
        {"Move Back 3", "Mundur tiga langkah.", CardKind::Instant, CardEffect::MoveBackThree, 3, ORANGE},
    };
}

std::vector<CardInfo> GUICardController::communityDeck() const {
    return {
        {"Birthday Card", "Tambah uang.", CardKind::Instant, CardEffect::GainMoney, 150, GREEN},
        {"Doctor Fee", "Kurangi uang.", CardKind::Instant, CardEffect::LoseMoney, 75, MAROON},
        {"Shield", "Lindungi dari jail.", CardKind::Hand, CardEffect::ActivateShield, 0, DARKBLUE},
    };
}

void GUICardController::applyCardEffect(const CardInfo& card, bool fromHand) {
    (void)card;
    (void)fromHand;
}
