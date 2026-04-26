#pragma once

#include "controller/GUIController/GUICardController.hpp"
#include "controller/GUIController/GUICommandController.hpp"
#include "controller/GUIController/GUIControllerContext.hpp"
#include "controller/GUIController/GUITileController.hpp"
#include "model/Game.hpp"
#include "view/raylib/GuiTypes.hpp"

#include <memory>
#include <string>
#include <vector>

class Card;
class ChanceCard;
class CommunityChestCard;
class OwnableTile;
class Player;
class StreetTile;
class Tile;

class GUIGameController {
public:
    GUIGameController();

    view::raylibgui::AppState& state();
    const view::raylibgui::AppState& state() const;

    GUICardController& cards();
    const GUICardController& cards() const;
    GUICommandController& commands();
    const GUICommandController& commands() const;
    GUITileController& tiles();
    const GUITileController& tiles() const;

    void tick(float deltaTime);
    void handleGlobalShortcuts();

    void openOverlay(view::raylibgui::OverlayType type);
    void closeOverlay();
    bool isOverlayOpen() const;

    int getMortgageValue(const view::raylibgui::TileInfo& tile) const;
    int getRedeemCost(const view::raylibgui::TileInfo& tile) const;
    int getSellToBankValue(const view::raylibgui::TileInfo& tile) const;
    int findJailIndex() const;
    int computeRent(const view::raylibgui::TileInfo& tile) const;
    int computeTileAssetValue(const view::raylibgui::TileInfo& tile) const;
    int computeNetWorth(int playerIndex) const;
    int jailFineAmount() const;
    int gridCellCount() const;
    Rectangle boardTileRect(Rectangle square, int index) const;

    void setSelectedTile(int tileIndex);
    void openTileDetail(int tileIndex);
    void openPurchase(int tileIndex);
    void openAuctionForTile(int tileIndex);
    void openIncomeTax();
    void openLuxuryTax();
    void openFestival();
    void openJail();
    void openCards();
    void openPortfolio();
    void openBuild();
    void openMortgage();
    void openRedeem();
    void openSave();
    void openLogs();
    void openSetDice();
    void openHelp();
    void openForceDrop();
    void openLiquidation();
    void openRandomCardDraw(int deckKey);
    void openGameOver();

    void startFreshSession();
    void loadSessionFromSlot(int slotIndex);
    void startTurn();
    void rollDice();
    void applyManualDice();
    void endTurn();
    void triggerTileEvent(int tileIndex, bool fromMovement = false);
    bool canSaveNow() const;
    void adjustManualDie(int dieIndex, int delta);

    bool canCurrentPlayerAffordSelectedPurchase() const;
    int currentPurchasePrice() const;
    void buySelectedProperty();
    void skipSelectedPurchase();

    void auctionPlaceBid(int targetBid);
    void auctionPass();
    void finalizeAuction();

    int flatIncomeTax() const;
    int percentageIncomeTax() const;
    void payIncomeTax(bool useFlatTax);
    int luxuryTaxAmount() const;
    void payLuxuryTax();

    void activateFestivalOnSelectedTile();
    void payJailFine();
    void attemptJailRoll();
    void useJailCard();

    void useSelectedHandCard();
    void storeDrawnCard();
    void applyDrawnCard();

    void buildOnSelectedTile();
    void mortgageSelectedTile();
    void redeemSelectedTile();
    void saveSession();
    void dropSelectedHandCard();
    void liquidateSelectedTile();
    void mortgageLiquidationSelectedTile();
    void declareBankrupt();
    bool canUseHandCardNow() const;
    bool isLiquidationRequired() const;

    void syncViewFromBackend();

    std::vector<int> currentPlayerStreetOptions() const;
    std::vector<int> currentPlayerOwnableOptions() const;
    std::vector<int> currentPlayerBuildOptions() const;
    std::vector<int> currentPlayerMortgageOptions() const;
    std::vector<int> currentPlayerRedeemOptions() const;

private:
    view::raylibgui::AppState appState_;
    Game backendGame_;
    bool guiTurnStarted_ = false;
    bool diceRolledThisTurn_ = false;
    float computerActionCooldown_ = 0.0f;
    std::shared_ptr<ChanceCard> pendingChanceCard_;
    std::shared_ptr<CommunityChestCard> pendingCommunityChestCard_;
    GUIControllerContext controllerContext_;
    GUICardController cardController_;
    GUICommandController commandController_;
    GUITileController tileController_;

    void addToast(const std::string& text, Color accent, float duration = 3.2f);
    void updateToasts(float deltaTime);
    void addLog(const std::string& actor, const std::string& action, const std::string& detail);
    void maybeOpenLiquidation();
    bool driveComputerTurn();
    bool handleComputerOverlay();
    bool handleComputerFreeTurn();
    bool canComputerUseCard(const Player& player, int cardIndex) const;
    bool prepareComputerCard(Player& player, int cardIndex);
    bool isBackendPlayerComputer(int playerIndex) const;
    int activeLiquidationPlayerIndex() const;

    int uiTileIndexFromBackend(int backendIndex) const;
    int backendTileIndexFromUi(int uiIndex) const;
    int normalizedBackendTileIndex(int backendIndex) const;
    int backendPlayerIndex(const Player* player) const;
    int currentBackendPlayerIndex() const;
    Tile* tileFromUi(int uiIndex) const;
    OwnableTile* ownableFromUi(int uiIndex) const;
    StreetTile* streetFromUi(int uiIndex) const;

    view::raylibgui::TileKind toGuiTileKind(const Tile& tile) const;
    view::raylibgui::TileInfo makeTileInfoFromBackend(const Tile& tile) const;
    view::raylibgui::PlayerInfo makePlayerInfoFromBackend(const Player& player, int playerIndex) const;
    view::raylibgui::CardInfo makeCardInfoFromBackend(const Card& card) const;
    view::raylibgui::LogItem makeLogItemFromBackend(const LogManager::LogEntry& entry) const;

    std::vector<view::raylibgui::SaveSlot> createInitialSaveSlots() const;
};
