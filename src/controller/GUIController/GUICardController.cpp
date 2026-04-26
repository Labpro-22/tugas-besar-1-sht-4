#include "controller/GUIController/GUICardController.hpp"

#include "model/Game.hpp"
#include "model/cards/ChanceCard.hpp"
#include "model/cards/CommunityChestCard.hpp"
#include "model/cards/DemolitionCard.hpp"
#include "model/cards/HandCard.hpp"
#include "model/cards/LassoCard.hpp"
#include "model/cards/MoveCard.hpp"
#include "model/cards/ShieldCard.hpp"
#include "model/cards/TeleportCard.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/Tile.hpp"

#include <algorithm>
#include <exception>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

using namespace view::raylibgui;

GUICardController::GUICardController(GUIControllerContext& controller)
    : controller_(controller) {
    controller_.openCardsCallback = [this]() { openCards(); };
    controller_.openRandomCardDrawCallback = [this](int deckKey) { openRandomCardDraw(deckKey); };
    controller_.closeCardDrawOverlayCallback = [this](bool discardPendingCard) { closeCardDrawOverlay(discardPendingCard); };
    controller_.discardAllCardsCallback = [this](Player& player) { discardAllCards(player); };
}

void GUICardController::openCards() {
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.appState_.getOverlay().setSelectedPlayer(-1);
    controller_.openOverlay(OverlayType::Cards);
}

void GUICardController::openRandomCardDraw(int deckKey) {
    clearPendingDrawnCard(true);
    try {
        if (deckKey == kCommunityDeckKey) {
            controller_.pendingCommunityChestCard_ = controller_.backendGame_.getCardManager().drawCommunityChestCard();
            if (controller_.pendingCommunityChestCard_ == nullptr) {
                controller_.addToast("Deck Dana Umum kosong.", RED);
                return;
            }
            controller_.appState_.getOverlay().setCard(controller_.makeCardInfoFromBackend(*controller_.pendingCommunityChestCard_));
        } else {
            controller_.pendingChanceCard_ = controller_.backendGame_.getCardManager().drawChanceCard();
            if (controller_.pendingChanceCard_ == nullptr) {
                controller_.addToast("Deck Chance kosong.", RED);
                return;
            }
            controller_.appState_.getOverlay().setCard(controller_.makeCardInfoFromBackend(*controller_.pendingChanceCard_));
        }
        controller_.appState_.getOverlay().setDeckKey(deckKey);
        controller_.openOverlay(OverlayType::CardDraw);
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        clearPendingDrawnCard(true);
    }
}

void GUICardController::useSelectedHandCard() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        std::vector<std::shared_ptr<HandCard>> cards = controller_.backendGame_.getCardManager().getHandCards(player);
        if (cards.empty()) {
            controller_.addToast("Belum ada kartu di tangan.", RED);
            return;
        }
        if (player.hasUsedHandCardThisTurn()) {
            controller_.addToast("Kartu kemampuan sudah dipakai pada turn ini.", RED);
            return;
        }
        if (controller_.backendGame_.getTurnManager().hasDiceRolledOnceThisTurn()) {
            controller_.addToast("Kartu hanya bisa dipakai di awal giliran sebelum lempar dadu.", RED);
            return;
        }

        const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(cards.size()) - 1));
        const int previousPosition = player.getPosition();
        const bool shouldResolveLanding =
            std::dynamic_pointer_cast<TeleportCard>(cards.at(selected)) != nullptr ||
            std::dynamic_pointer_cast<MoveCard>(cards.at(selected)) != nullptr;
        if (!configureSelectedHandCard(player, selected)) {
            controller_.addToast("Pilih target kartu yang valid terlebih dahulu.", RED);
            return;
        }
        controller_.backendGame_.getCardManager().useHandCard(controller_.backendGame_, player, selected);
        controller_.backendGame_.getTurnManager().registerAction();

        const int normalizedPosition = controller_.normalizedBackendTileIndex(player.getPosition());
        player.moveTo(normalizedPosition);
        controller_.addToast("Kartu digunakan.", SKYBLUE);
        controller_.closeOverlay();
        if (shouldResolveLanding || normalizedPosition != controller_.normalizedBackendTileIndex(previousPosition)) {
            controller_.resolveBackendLanding(normalizedPosition, true);
        } else {
            controller_.syncViewFromBackend();
            controller_.finishTurnAfterDiceIfReady();
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUICardController::storeDrawnCard() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        std::shared_ptr<HandCard> handCard;
        std::string actionType = "KARTU";
        if (controller_.pendingChanceCard_ != nullptr) {
            handCard = std::dynamic_pointer_cast<HandCard>(controller_.pendingChanceCard_);
            if (handCard == nullptr) {
                controller_.addToast("Kartu ini harus langsung diterapkan.", RED);
                return;
            }
            actionType = "KSP";
            controller_.pendingChanceCard_.reset();
        } else if (controller_.pendingCommunityChestCard_ != nullptr) {
            handCard = std::dynamic_pointer_cast<HandCard>(controller_.pendingCommunityChestCard_);
            if (handCard == nullptr) {
                controller_.addToast("Kartu ini harus langsung diterapkan.", RED);
                return;
            }
            actionType = "DNU";
            controller_.pendingCommunityChestCard_.reset();
        } else {
            controller_.closeOverlay();
            return;
        }

        player.addHandCard(handCard);
        controller_.addLog(player.getUsername(), actionType, "Mengambil kartu " + handCard->getName() + " dan menyimpannya ke tangan.");
        controller_.addToast("Kartu tersimpan di tangan.", SKYBLUE);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        if (controller_.backendGame_.getCardManager().needsForceDrop(player)) {
            controller_.openForceDrop();
        } else {
            controller_.finishTurnAfterDiceIfReady();
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUICardController::applyDrawnCard() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        const int previousPosition = player.getPosition();
        if (controller_.pendingChanceCard_ != nullptr) {
            controller_.addLog(player.getUsername(), "KSP", "Mengambil kartu " + controller_.pendingChanceCard_->getName() + ".");
            controller_.pendingChanceCard_->apply(controller_.backendGame_, player);
            controller_.backendGame_.getCardManager().discardChanceCard(controller_.pendingChanceCard_);
            controller_.pendingChanceCard_.reset();
        } else if (controller_.pendingCommunityChestCard_ != nullptr) {
            controller_.addLog(player.getUsername(), "DNU", "Mengambil kartu " + controller_.pendingCommunityChestCard_->getName() + ".");
            controller_.pendingCommunityChestCard_->apply(controller_.backendGame_, player);
            controller_.backendGame_.getCardManager().discardCommunityChestCard(controller_.pendingCommunityChestCard_);
            controller_.pendingCommunityChestCard_.reset();
        } else {
            controller_.closeOverlay();
            return;
        }

        const int normalizedPosition = controller_.normalizedBackendTileIndex(player.getPosition());
        player.moveTo(normalizedPosition);
        controller_.closeOverlay();
        if (normalizedPosition != controller_.normalizedBackendTileIndex(previousPosition)) {
            controller_.resolveBackendLanding(normalizedPosition, true);
        } else {
            controller_.syncViewFromBackend();
            controller_.maybeOpenLiquidation();
            controller_.finishTurnAfterDiceIfReady();
        }
    } catch (const std::exception& exception) {
        clearPendingDrawnCard(true);
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
    }
}

void GUICardController::dropSelectedHandCard() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        const bool voluntaryDrop = controller_.appState_.getOverlay().getType() == OverlayType::Cards;
        if (voluntaryDrop && controller_.backendGame_.getTurnManager().hasDiceRolledOnceThisTurn()) {
            controller_.addToast("Kartu hanya bisa dibuang sebelum lempar dadu.", RED);
            return;
        }
        const int cardCount = player.countCards();
        if (cardCount <= 0) {
            controller_.closeOverlay();
            return;
        }

        const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), cardCount - 1));
        std::vector<std::shared_ptr<HandCard>> cards = controller_.backendGame_.getCardManager().getHandCards(player);
        std::string cardName = selected < static_cast<int>(cards.size()) && cards.at(selected) != nullptr
            ? cards.at(selected)->getName()
            : "Kartu";
        controller_.backendGame_.getCardManager().dropHandCard(player, selected);
        if (voluntaryDrop) {
            controller_.backendGame_.getTurnManager().registerAction();
        }
        controller_.addLog(player.getUsername(), "DROP_CARD", "Membuang " + cardName + ".");
        controller_.addToast("Kartu dibuang.", SKYBLUE);
        controller_.syncViewFromBackend();
        if (!controller_.backendGame_.getCardManager().needsForceDrop(player)) {
            controller_.closeOverlay();
            controller_.finishTurnAfterDiceIfReady();
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUICardController::useJailCard() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        std::vector<std::shared_ptr<HandCard>> cards = controller_.backendGame_.getCardManager().getHandCards(player);
        int shieldIndex = -1;
        for (int index = 0; index < static_cast<int>(cards.size()); index++) {
            if (std::dynamic_pointer_cast<ShieldCard>(cards.at(index)) != nullptr) {
                shieldIndex = index;
                break;
            }
        }

        if (shieldIndex < 0) {
            controller_.addToast("Tidak ada kartu shield untuk keluar jail.", RED);
            return;
        }

        controller_.backendGame_.getCardManager().dropHandCard(player, shieldIndex);
        controller_.backendGame_.getJailManager().releaseFromJail(player);
        controller_.addLog(player.getUsername(), "JAIL", "Menggunakan ShieldCard untuk keluar dari jail.");
        controller_.addToast("Keluar dari jail dengan kartu.", SKYBLUE);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

std::vector<int> GUICardController::teleportTargetOptions() const {
    std::vector<int> targets;
    for (const std::shared_ptr<Tile>& tile : controller_.backendGame_.getBoard().getTiles()) {
        if (tile != nullptr) {
            targets.push_back(controller_.uiTileIndexFromBackend(tile->getIndex()));
        }
    }
    std::sort(targets.begin(), targets.end());
    return targets;
}

std::vector<int> GUICardController::demolitionTargetOptions() const {
    std::vector<int> targets;
    if (controller_.backendGame_.getPlayers().empty()) {
        return targets;
    }

    const Player& player = controller_.backendGame_.getCurrentPlayer();
    for (const Player& opponent : controller_.backendGame_.getPlayers()) {
        if (&opponent == &player || opponent.isBankrupt()) {
            continue;
        }

        for (OwnableTile* property : controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), opponent)) {
            StreetTile* street = dynamic_cast<StreetTile*>(property);
            if (street != nullptr && street->getBuildingLevel() > 0) {
                targets.push_back(controller_.uiTileIndexFromBackend(street->getIndex()));
            }
        }
    }

    std::sort(targets.begin(), targets.end());
    return targets;
}

bool GUICardController::currentPlayerNeedsForceDrop() const {
    if (controller_.backendGame_.getPlayers().empty()) {
        return false;
    }
    return controller_.backendGame_.getCardManager().needsForceDrop(controller_.backendGame_.getCurrentPlayer());
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

void GUICardController::closeCardDrawOverlay(bool discardPendingCard) {
    if (controller_.appState_.getOverlay().getType() == OverlayType::CardDraw) {
        clearPendingDrawnCard(discardPendingCard);
    }
}

void GUICardController::clearPendingDrawnCard(bool discardPendingCard) {
    if (discardPendingCard && controller_.pendingChanceCard_ != nullptr) {
        controller_.backendGame_.getCardManager().discardChanceCard(controller_.pendingChanceCard_);
    }
    if (discardPendingCard && controller_.pendingCommunityChestCard_ != nullptr) {
        controller_.backendGame_.getCardManager().discardCommunityChestCard(controller_.pendingCommunityChestCard_);
    }
    controller_.pendingChanceCard_.reset();
    controller_.pendingCommunityChestCard_.reset();
}

void GUICardController::discardAllCards(Player& player) {
    while (player.countCards() > 0) {
        controller_.backendGame_.getCardManager().dropHandCard(player, 0);
    }
}

bool GUICardController::configureSelectedHandCard(Player& player, int cardIndex) {
    std::vector<std::shared_ptr<HandCard>> cards = controller_.backendGame_.getCardManager().getHandCards(player);
    if (cardIndex < 0 || cardIndex >= static_cast<int>(cards.size()) || cards.at(cardIndex) == nullptr) {
        return false;
    }

    if (std::shared_ptr<TeleportCard> teleport = std::dynamic_pointer_cast<TeleportCard>(cards.at(cardIndex))) {
        const std::vector<int> targets = teleportTargetOptions();
        if (targets.empty()) {
            return false;
        }

        int targetOption = controller_.appState_.getOverlay().getSelectedPlayer();
        if (player.isComputerPlayer()) {
            const int selectedTile = controller_.appState_.getGame().getSelectedTile();
            const auto selected = std::find(targets.begin(), targets.end(), selectedTile);
            targetOption = selected != targets.end()
                ? static_cast<int>(std::distance(targets.begin(), selected))
                : 0;
        }
        if (targetOption < 0 || targetOption >= static_cast<int>(targets.size())) {
            return false;
        }

        teleport->setTargetTileIndex(controller_.backendTileIndexFromUi(targets.at(targetOption)));
        return true;
    }

    if (std::shared_ptr<LassoCard> lasso = std::dynamic_pointer_cast<LassoCard>(cards.at(cardIndex))) {
        const int myPosition = player.getPosition();
        const int boardSize = controller_.backendGame_.getBoard().getBoardSize();
        for (Player& candidate : controller_.backendGame_.getPlayers()) {
            if (&candidate == &player || candidate.isBankrupt() || candidate.isJailed()) {
                continue;
            }
            const int otherPosition = candidate.getPosition();
            const bool isAhead = (otherPosition > myPosition) ||
                                 (otherPosition < myPosition && boardSize > 0 && (myPosition - otherPosition) > boardSize / 2);
            if (isAhead) {
                lasso->setTargetPlayer(&candidate);
                return true;
            }
        }
        return false;
    }

    if (std::shared_ptr<DemolitionCard> demolition = std::dynamic_pointer_cast<DemolitionCard>(cards.at(cardIndex))) {
        const std::vector<int> targets = demolitionTargetOptions();
        if (targets.empty()) {
            return false;
        }

        int targetOption = controller_.appState_.getOverlay().getSelectedPlayer();
        if (player.isComputerPlayer()) {
            targetOption = 0;
        }
        if (targetOption < 0 || targetOption >= static_cast<int>(targets.size())) {
            return false;
        }

        StreetTile* targetStreet = controller_.streetFromUi(targets.at(targetOption));
        if (targetStreet == nullptr ||
            targetStreet->getOwner() == nullptr ||
            targetStreet->getOwner() == &player ||
            targetStreet->getBuildingLevel() <= 0) {
            return false;
        }

        demolition->setTargetStreet(targetStreet);
        return true;
    }

    return true;
}
