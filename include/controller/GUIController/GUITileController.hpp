#pragma once

#include "view/raylib/GuiTypes.hpp"

#include <vector>

class GUIGameController;

class GUITileController {
public:
    explicit GUITileController(GUIGameController& controller);

    int getMortgageValue(const view::raylibgui::TileInfo& tile) const;
    int getRedeemCost(const view::raylibgui::TileInfo& tile) const;
    int findJailIndex() const;
    int computeRent(const view::raylibgui::TileInfo& tile) const;
    int computeTileAssetValue(const view::raylibgui::TileInfo& tile) const;
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
    GUIGameController& controller_;
};
