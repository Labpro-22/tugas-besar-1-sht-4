#pragma once

#include "controller/GUIController/GUIControllerContext.hpp"

#include <vector>

class Player;

class GUITileController {
public:
    explicit GUITileController(GUIControllerContext& context);

    int getMortgageValue(const view::raylibgui::TileInfo& tile) const;
    int getRedeemCost(const view::raylibgui::TileInfo& tile) const;
    int findJailIndex() const;
    int computeRent(const view::raylibgui::TileInfo& tile) const;
    int computeTileAssetValue(const view::raylibgui::TileInfo& tile) const;
    int computeNetWorth(int playerIndex) const;
    int jailFineAmount() const;
    Rectangle boardTileRect(Rectangle square, int index) const;

    void setSelectedTile(int tileIndex);
    void openTileDetail(int tileIndex);
    void openPurchase(int tileIndex);
    void openAuctionForTile(int tileIndex);
    void openIncomeTax();
    void openLuxuryTax();
    void openFestival();
    void openJail();
    void openBuild();
    void openMortgage();
    void openRedeem();
    void openLiquidation();

    void triggerTileEvent(int tileIndex, bool fromMovement = false);

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
    void attemptJailRoll();

    void buildOnSelectedTile();
    void mortgageSelectedTile();
    void redeemSelectedTile();
    void liquidateSelectedTile();
    void declareBankrupt();

    std::vector<int> currentPlayerStreetOptions() const;
    std::vector<int> currentPlayerBuildOptions() const;
    std::vector<int> currentPlayerMortgageOptions() const;
    std::vector<int> currentPlayerRedeemOptions() const;

private:
    GUIControllerContext& controller_;

    int moveBackendPlayer(Player& player, int steps);
    void resolveBackendLanding(int backendTileIndex, bool fromMovement = false);
};
