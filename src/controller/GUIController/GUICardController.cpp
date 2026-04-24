#include "controller/GUIController/GUICardController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

#include <algorithm>
#include <cctype>
#include <string>

using namespace view::raylibgui;

GUICardController::GUICardController(GUIGameController& controller)
    : controller_(controller) {}

void GUICardController::openCards() {
    controller_.state().getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Cards);
}

void GUICardController::openRandomCardDraw(int deckKey) {
    controller_.state().getOverlay().setDeckKey(deckKey);
    controller_.state().getOverlay().setCard(pickRandomCard(deckKey));
    controller_.openOverlay(OverlayType::CardDraw);
}

void GUICardController::useSelectedHandCard() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.getHandCards().empty()) {
        controller_.addToast("Belum ada kartu di tangan.", RED);
        return;
    }

    const int selected = std::max(0, std::min(controller_.state().getOverlay().getSelectedIndex(), static_cast<int>(player.getHandCards().size()) - 1));
    const CardInfo card = player.getHandCards().at(selected);
    player.getHandCards().erase(player.getHandCards().begin() + selected);
    controller_.closeOverlay();
    applyCardEffect(card, true);
}

void GUICardController::storeDrawnCard() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const CardInfo card = controller_.state().getOverlay().getCard();
    player.getHandCards().push_back(card);
    controller_.addLog(player.getName(), "Draw Card", "Menyimpan kartu " + card.getTitle() + " ke tangan.");
    controller_.addToast("Kartu disimpan ke tangan.", card.getAccent());
    controller_.closeOverlay();
    maybeOpenForceDrop();
}

void GUICardController::applyDrawnCard() {
    const CardInfo card = controller_.state().getOverlay().getCard();
    controller_.closeOverlay();
    applyCardEffect(card, false);
}

void GUICardController::dropSelectedHandCard() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.getHandCards().empty()) {
        controller_.closeOverlay();
        return;
    }

    const int selected = std::max(0, std::min(controller_.state().getOverlay().getSelectedIndex(), static_cast<int>(player.getHandCards().size()) - 1));
    const CardInfo discarded = player.getHandCards().at(selected);
    player.getHandCards().erase(player.getHandCards().begin() + selected);
    controller_.addLog(player.getName(), "Drop Card", "Membuang kartu " + discarded.getTitle() + ".");
    controller_.addToast("Kartu dibuang.", discarded.getAccent());
    if (player.getHandCards().size() <= 3U) {
        controller_.closeOverlay();
    } else {
        controller_.state().getOverlay().setSelectedIndex(std::min(selected, static_cast<int>(player.getHandCards().size()) - 1));
    }
}

void GUICardController::useJailCard() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (!player.isJailed()) {
        controller_.addToast("Pemain ini tidak sedang di jail.", RED);
        return;
    }

    auto isJailCard = [](const CardInfo& card) {
        std::string title = card.getTitle();
        std::transform(title.begin(), title.end(), title.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return card.getEffect() == CardEffect::ActivateShield ||
               title.find("jail") != std::string::npos ||
               title.find("bebas") != std::string::npos;
    };

    const auto found = std::find_if(player.getHandCards().begin(), player.getHandCards().end(), isJailCard);
    if (found == player.getHandCards().end()) {
        controller_.addToast("Tidak ada kartu bebas jail yang bisa dipakai.", RED);
        return;
    }

    const CardInfo usedCard = *found;
    player.getHandCards().erase(found);
    player.setJailed(false);
    player.setFailedJailRolls(0);
    controller_.addLog(player.getName(), "Use Jail Card", "Menggunakan " + usedCard.getTitle() + " untuk keluar dari jail.");
    controller_.addToast("Keluar dari jail dengan kartu.", usedCard.getAccent());
    controller_.closeOverlay();
}

bool GUICardController::currentPlayerNeedsForceDrop() const {
    const GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return false;
    }

    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    return player.getHandCards().size() > 3U;
}

void GUICardController::maybeOpenForceDrop() {
    if (!currentPlayerNeedsForceDrop()) {
        return;
    }

    controller_.state().getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::ForceDrop);
}

CardInfo GUICardController::pickRandomCard(int deckKey) {
    const std::vector<CardInfo> cards = deckKey == kCommunityDeckKey ? communityDeck() : chanceDeck();
    if (cards.empty()) {
        return {};
    }

    std::uniform_int_distribution<int> distribution(0, static_cast<int>(cards.size()) - 1);
    return cards.at(distribution(controller_.state().getRng()));
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
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    controller_.addLog(player.getName(), fromHand ? "Use Card" : "Apply Card", card.getTitle() + ": " + card.getDescription());

    switch (card.getEffect()) {
        case CardEffect::GainMoney:
            player.setMoney(player.getMoney() + card.getMagnitude());
            controller_.addToast("Uang bertambah " + std::to_string(card.getMagnitude()) + ".", card.getAccent());
            break;
        case CardEffect::LoseMoney:
            player.setMoney(player.getMoney() - card.getMagnitude());
            controller_.addToast("Uang berkurang " + std::to_string(card.getMagnitude()) + ".", card.getAccent());
            controller_.maybeOpenLiquidation();
            break;
        case CardEffect::MoveToGo:
            player.setPosition(0);
            player.setMoney(player.getMoney() + 200);
            controller_.addToast("Pindah ke GO.", card.getAccent());
            controller_.triggerTileEvent(0, true);
            break;
        case CardEffect::MoveBackThree: {
            const int boardSize = static_cast<int>(game.getBoard().size());
            if (boardSize <= 0) {
                break;
            }
            const int nextPosition = (player.getPosition() - std::max(1, card.getMagnitude()) + boardSize) % boardSize;
            player.setPosition(nextPosition);
            controller_.addToast("Mundur ke " + game.getBoard().at(nextPosition).getName() + ".", card.getAccent());
            controller_.triggerTileEvent(nextPosition, true);
            break;
        }
        case CardEffect::GoToJail:
            if (player.isShieldActive()) {
                player.setShieldActive(false);
                controller_.addToast("Shield aktif menahan efek jail.", card.getAccent());
                break;
            }
            player.setJailed(true);
            player.setFailedJailRolls(0);
            player.setPosition(controller_.findJailIndex());
            controller_.addToast("Masuk jail.", RED);
            controller_.openJail();
            break;
        case CardEffect::TeleportAnywhere: {
            const int destination = std::max(0, std::min(game.getSelectedTile(), static_cast<int>(game.getBoard().size()) - 1));
            player.setPosition(destination);
            controller_.addToast("Teleport ke " + game.getBoard().at(destination).getName() + ".", card.getAccent());
            controller_.triggerTileEvent(destination, true);
            break;
        }
        case CardEffect::ActivateShield:
            if (player.isJailed()) {
                player.setJailed(false);
                player.setFailedJailRolls(0);
                controller_.addToast("Kartu membebaskan pemain dari jail.", card.getAccent());
            } else {
                player.setShieldActive(true);
                controller_.addToast("Shield aktif untuk pemain ini.", card.getAccent());
            }
            break;
        case CardEffect::ActivateDiscount:
            player.setDiscountPercent(card.getMagnitude() > 0 ? card.getMagnitude() : 20);
            controller_.addToast("Diskon pembelian aktif.", card.getAccent());
            break;
    }
}
