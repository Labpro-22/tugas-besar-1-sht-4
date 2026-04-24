#include "controller/GUIController/GUIControllerContext.hpp"

#include "model/Player.hpp"

using namespace view::raylibgui;

GUIControllerContext::GUIControllerContext(
    AppState& appState,
    Game& backendGame,
    bool& guiTurnStarted,
    bool& diceRolledThisTurn,
    std::shared_ptr<ChanceCard>& pendingChanceCard,
    std::shared_ptr<CommunityChestCard>& pendingCommunityChestCard,
    std::function<void(const std::string&, Color, float)> addToast,
    std::function<void(float)> updateToasts,
    std::function<void(const std::string&, const std::string&, const std::string&)> addLog,
    std::function<void()> maybeOpenLiquidation,
    std::function<void()> syncViewFromBackend,
    std::function<int(int)> uiTileIndexFromBackend,
    std::function<int(int)> backendTileIndexFromUi,
    std::function<int(int)> normalizedBackendTileIndex,
    std::function<int()> currentBackendPlayerIndex,
    std::function<OwnableTile*(int)> ownableFromUi,
    std::function<StreetTile*(int)> streetFromUi,
    std::function<TileKind(const Tile&)> toGuiTileKind,
    std::function<CardInfo(const Card&)> makeCardInfoFromBackend
) : appState_(appState),
    backendGame_(backendGame),
    guiTurnStarted_(guiTurnStarted),
    diceRolledThisTurn_(diceRolledThisTurn),
    pendingChanceCard_(pendingChanceCard),
    pendingCommunityChestCard_(pendingCommunityChestCard),
    addToast_(std::move(addToast)),
    updateToasts_(std::move(updateToasts)),
    addLog_(std::move(addLog)),
    maybeOpenLiquidation_(std::move(maybeOpenLiquidation)),
    syncViewFromBackend_(std::move(syncViewFromBackend)),
    uiTileIndexFromBackend_(std::move(uiTileIndexFromBackend)),
    backendTileIndexFromUi_(std::move(backendTileIndexFromUi)),
    normalizedBackendTileIndex_(std::move(normalizedBackendTileIndex)),
    currentBackendPlayerIndex_(std::move(currentBackendPlayerIndex)),
    ownableFromUi_(std::move(ownableFromUi)),
    streetFromUi_(std::move(streetFromUi)),
    toGuiTileKind_(std::move(toGuiTileKind)),
    makeCardInfoFromBackend_(std::move(makeCardInfoFromBackend)) {}

void GUIControllerContext::addToast(const std::string& text, Color accent, float duration) const {
    addToast_(text, accent, duration);
}

void GUIControllerContext::updateToasts(float deltaTime) const {
    updateToasts_(deltaTime);
}

void GUIControllerContext::addLog(const std::string& actor, const std::string& action, const std::string& detail) const {
    addLog_(actor, action, detail);
}

void GUIControllerContext::maybeOpenLiquidation() const {
    maybeOpenLiquidation_();
}

void GUIControllerContext::syncViewFromBackend() const {
    syncViewFromBackend_();
}

int GUIControllerContext::uiTileIndexFromBackend(int backendIndex) const {
    return uiTileIndexFromBackend_(backendIndex);
}

int GUIControllerContext::backendTileIndexFromUi(int uiIndex) const {
    return backendTileIndexFromUi_(uiIndex);
}

int GUIControllerContext::normalizedBackendTileIndex(int backendIndex) const {
    return normalizedBackendTileIndex_(backendIndex);
}

int GUIControllerContext::currentBackendPlayerIndex() const {
    return currentBackendPlayerIndex_();
}

OwnableTile* GUIControllerContext::ownableFromUi(int uiIndex) const {
    return ownableFromUi_(uiIndex);
}

StreetTile* GUIControllerContext::streetFromUi(int uiIndex) const {
    return streetFromUi_(uiIndex);
}

TileKind GUIControllerContext::toGuiTileKind(const Tile& tile) const {
    return toGuiTileKind_(tile);
}

CardInfo GUIControllerContext::makeCardInfoFromBackend(const Card& card) const {
    return makeCardInfoFromBackend_(card);
}

void GUIControllerContext::openOverlay(OverlayType type) const {
    if (openOverlayCallback) {
        openOverlayCallback(type);
    }
}

void GUIControllerContext::closeOverlay() const {
    if (closeOverlayCallback) {
        closeOverlayCallback();
    }
}

void GUIControllerContext::openCards() const {
    if (openCardsCallback) {
        openCardsCallback();
    }
}

void GUIControllerContext::openRandomCardDraw(int deckKey) const {
    if (openRandomCardDrawCallback) {
        openRandomCardDrawCallback(deckKey);
    }
}

void GUIControllerContext::openForceDrop() const {
    if (openForceDropCallback) {
        openForceDropCallback();
    }
}

void GUIControllerContext::openGameOver() const {
    if (openGameOverCallback) {
        openGameOverCallback();
    }
}

void GUIControllerContext::openJail() const {
    if (openJailCallback) {
        openJailCallback();
    }
}

void GUIControllerContext::endTurn() const {
    if (endTurnCallback) {
        endTurnCallback();
    }
}

void GUIControllerContext::finishTurnAfterDiceIfReady() const {
    if (finishTurnAfterDiceIfReadyCallback) {
        finishTurnAfterDiceIfReadyCallback();
    }
}

int GUIControllerContext::moveBackendPlayer(Player& player, int steps) const {
    if (moveBackendPlayerCallback) {
        return moveBackendPlayerCallback(player, steps);
    }
    return player.getPosition();
}

void GUIControllerContext::resolveBackendLanding(int backendTileIndex, bool fromMovement) const {
    if (resolveBackendLandingCallback) {
        resolveBackendLandingCallback(backendTileIndex, fromMovement);
    }
}

void GUIControllerContext::closeCardDrawOverlay(bool discardPendingCard) const {
    if (closeCardDrawOverlayCallback) {
        closeCardDrawOverlayCallback(discardPendingCard);
    }
}

void GUIControllerContext::discardAllCards(Player& player) const {
    if (discardAllCardsCallback) {
        discardAllCardsCallback(player);
    }
}
