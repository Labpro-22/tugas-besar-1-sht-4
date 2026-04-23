#pragma once

#include "controller/GUIController/GUICardController.hpp"
#include "controller/GUIController/GUICommandController.hpp"
#include "controller/GUIController/GUITileController.hpp"
#include "view/raylib/GuiTypes.hpp"

#include <string>
#include <vector>

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
    int findJailIndex() const;
    int computeRent(const view::raylibgui::TileInfo& tile) const;
    int computeTileAssetValue(const view::raylibgui::TileInfo& tile) const;
    int computeNetWorth(int playerIndex) const;
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

    void auctionRaiseBid(int amount);
    void auctionPass();
    void finalizeAuction();

    int flatIncomeTax() const;
    int percentageIncomeTax() const;
    void payIncomeTax(bool useFlatTax);
    int luxuryTaxAmount() const;
    void payLuxuryTax();

    void activateFestivalOnSelectedTile();
    void payJailFine();
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
    void declareBankrupt();

    std::vector<int> currentPlayerStreetOptions() const;
    std::vector<int> currentPlayerBuildOptions() const;
    std::vector<int> currentPlayerMortgageOptions() const;
    std::vector<int> currentPlayerRedeemOptions() const;

private:
    friend class GUICardController;
    friend class GUICommandController;
    friend class GUITileController;

    view::raylibgui::AppState appState_;
    GUICardController cardController_;
    GUICommandController commandController_;
    GUITileController tileController_;

    void addToast(const std::string& text, Color accent, float duration = 3.2f);
    void updateToasts(float deltaTime);
    void addLog(const std::string& actor, const std::string& action, const std::string& detail);
    void maybeOpenLiquidation();

    view::raylibgui::GameState createBaseGame() const;
    void applyScenario(view::raylibgui::GameState& game, int scenarioIndex) const;
    std::vector<view::raylibgui::TileInfo> createBoard() const;
    std::vector<view::raylibgui::SaveSlot> createInitialSaveSlots() const;
};
