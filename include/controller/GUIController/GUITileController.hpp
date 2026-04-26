#pragma once

#include "controller/GUIController/GUIControllerContext.hpp"

#include <vector>

class Player;
enum class OwnershipStatus;

class GUITileController {
public:
    explicit GUITileController(GUIControllerContext& context);

    int getMortgageValue(const view::raylibgui::TileInfo& tile) const;
    int getRedeemCost(const view::raylibgui::TileInfo& tile) const;
    int getSellToBankValue(const view::raylibgui::TileInfo& tile) const;
    view::raylibgui::TileDeedInfo deedInfoForTile(const view::raylibgui::TileInfo& tile) const;
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
    void openBuild();
    void openMortgage();
    void openRedeem();
    void openLiquidation();

    void triggerTileEvent(int tileIndex, bool fromMovement = false);

    bool canCurrentPlayerAffordSelectedPurchase() const;
    bool canActiveAuctionBidderPass() const;
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

    void buildOnSelectedTile();
    void mortgageSelectedTile();
    void redeemSelectedTile();
    void liquidateSelectedTile();
    void mortgageLiquidationSelectedTile();
    void declareBankrupt();

    std::vector<int> currentPlayerStreetOptions() const;
    std::vector<int> currentPlayerOwnableOptions() const;
    std::vector<int> currentPlayerBuildOptions() const;
    std::vector<int> currentPlayerMortgageOptions() const;
    std::vector<int> currentPlayerRedeemOptions() const;

private:
    GUIControllerContext& controller_;

    int moveBackendPlayer(Player& player, int steps);
    void resolveBackendLanding(int backendTileIndex, bool fromMovement = false);
    Player* activeLiquidationPlayer() const;
    int activeLiquidationPlayerIndex() const;
    bool isCurrentTurnPlayer(const Player& player) const;
    size_t activePlayerCount() const;
    bool colorGroupHasBuildings(const StreetTile& tile) const;
    int sellColorGroupBuildings(Player& player, const StreetTile& tile) const;
    void returnPropertyToBank(OwnableTile& property) const;

    static Color playerAccent(int index);
    static std::string normalizeKey(std::string value);
    static std::string upperCopy(std::string value);
    static Color groupAccent(const std::string& group);
    static std::string colorDisplayName(const std::string& group);
    static std::string ownershipStatusText(OwnershipStatus status);
    static std::string deedOwnershipStatusText(const OwnableTile& tile);
    static std::string buildingText(const StreetTile& tile);
    static Color kindAccent(view::raylibgui::TileKind kind, const std::string& group);
};
