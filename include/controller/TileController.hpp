#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class Game;
class UIManager;
class Player;
class Tile;
class OwnableTile;
class StreetTile;
class IncomeTaxTile;
class LuxuryTaxTile;
class FestivalTile;
enum class OwnershipStatus;

class TileController {
private:
    Game& game;
    UIManager& uiManager;
    int pendingDebtAmount;
    Player* pendingCreditor;
    bool pendingDebtToBank;

    string toUpperCopy(string value) const;
    string ownershipStatusText(OwnershipStatus status) const;
    string normalizeColorKey(string value) const;
    string colorDisplayName(const string& colorGroup) const;
    string buildingText(const StreetTile& tile) const;
    string festivalStatus(const StreetTile& tile) const;
    int rentPreview(const StreetTile& tile) const;
    void buildDeedData(const OwnableTile& tile, string& title, int& purchasePrice, int& mortgageValue, vector<string>& moneyRowLabels, vector<int>& moneyRowValues, vector<string>& detailRowLabels, vector<string>& detailRowValues, string& ownershipStatus, string& ownerName) const;
    void returnPropertyToBank(OwnableTile& property) const;
    vector<OwnableTile*> ownedProperties(Game& game, Player& player) const;
    int liquidationValue(Game& game, Player& player) const;
    int discountedPurchasePrice(const Player& player, const OwnableTile& property) const;
    void acquireProperty(Player& player, OwnableTile& property, int price) const;
    size_t activePlayerCount(Game& game) const;

public:
    TileController(Game& game, UIManager& uiManager);
    TileController(const TileController& other);
    ~TileController();
    TileController& operator=(const TileController& other);

    void resolveLanding(Tile& tile, Player& player);

    void handleStreetPurchase(StreetTile& tile);
    void handleAuction(OwnableTile& tile, Player* triggerPlayer);
    void handleIncomeTax(IncomeTaxTile& tile);
    void handleLuxuryTax(LuxuryTaxTile& tile);
    void handleFestival(FestivalTile& tile);
    void handleForceDrop(Player& player);
    void handleJailTurn(Player& player);
    void handleBankruptcy(Player& player);
};
