#pragma once

#include "view/raylib/GuiTypes.hpp"

#include <functional>
#include <memory>
#include <string>

class Card;
class ChanceCard;
class CommunityChestCard;
class Game;
class OwnableTile;
class Player;
class StreetTile;
class Tile;

class GUIControllerContext {
public:
    view::raylibgui::AppState& appState_;
    Game& backendGame_;
    bool& guiTurnStarted_;
    bool& diceRolledThisTurn_;
    std::shared_ptr<ChanceCard>& pendingChanceCard_;
    std::shared_ptr<CommunityChestCard>& pendingCommunityChestCard_;

    GUIControllerContext(
        view::raylibgui::AppState& appState,
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
        std::function<view::raylibgui::TileKind(const Tile&)> toGuiTileKind,
        std::function<view::raylibgui::CardInfo(const Card&)> makeCardInfoFromBackend
    );

    void addToast(const std::string& text, Color accent, float duration = 3.2f) const;
    void updateToasts(float deltaTime) const;
    void addLog(const std::string& actor, const std::string& action, const std::string& detail) const;
    void maybeOpenLiquidation() const;
    void syncViewFromBackend() const;

    int uiTileIndexFromBackend(int backendIndex) const;
    int backendTileIndexFromUi(int uiIndex) const;
    int normalizedBackendTileIndex(int backendIndex) const;
    int currentBackendPlayerIndex() const;
    OwnableTile* ownableFromUi(int uiIndex) const;
    StreetTile* streetFromUi(int uiIndex) const;
    view::raylibgui::TileKind toGuiTileKind(const Tile& tile) const;
    view::raylibgui::CardInfo makeCardInfoFromBackend(const Card& card) const;

    void openOverlay(view::raylibgui::OverlayType type) const;
    void closeOverlay() const;
    void openCards() const;
    void openRandomCardDraw(int deckKey) const;
    void openForceDrop() const;
    void openGameOver() const;
    void openJail() const;
    void endTurn() const;
    void finishTurnAfterDiceIfReady() const;
    int moveBackendPlayer(Player& player, int steps) const;
    void resolveBackendLanding(int backendTileIndex, bool fromMovement = false) const;
    void closeCardDrawOverlay(bool discardPendingCard) const;
    void discardAllCards(Player& player) const;

    std::function<void(view::raylibgui::OverlayType)> openOverlayCallback;
    std::function<void()> closeOverlayCallback;
    std::function<void()> openCardsCallback;
    std::function<void(int)> openRandomCardDrawCallback;
    std::function<void()> openForceDropCallback;
    std::function<void()> openGameOverCallback;
    std::function<void()> openJailCallback;
    std::function<void()> endTurnCallback;
    std::function<void()> finishTurnAfterDiceIfReadyCallback;
    std::function<int(Player&, int)> moveBackendPlayerCallback;
    std::function<void(int, bool)> resolveBackendLandingCallback;
    std::function<void(bool)> closeCardDrawOverlayCallback;
    std::function<void(Player&)> discardAllCardsCallback;

private:
    std::function<void(const std::string&, Color, float)> addToast_;
    std::function<void(float)> updateToasts_;
    std::function<void(const std::string&, const std::string&, const std::string&)> addLog_;
    std::function<void()> maybeOpenLiquidation_;
    std::function<void()> syncViewFromBackend_;
    std::function<int(int)> uiTileIndexFromBackend_;
    std::function<int(int)> backendTileIndexFromUi_;
    std::function<int(int)> normalizedBackendTileIndex_;
    std::function<int()> currentBackendPlayerIndex_;
    std::function<OwnableTile*(int)> ownableFromUi_;
    std::function<StreetTile*(int)> streetFromUi_;
    std::function<view::raylibgui::TileKind(const Tile&)> toGuiTileKind_;
    std::function<view::raylibgui::CardInfo(const Card&)> makeCardInfoFromBackend_;
};
