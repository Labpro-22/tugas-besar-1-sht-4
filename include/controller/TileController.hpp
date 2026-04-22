#pragma once

#include <memory>

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

class TileController {
private:
    Game& game;
    UIManager& uiManager;
    int pendingDebtAmount;
    Player* pendingCreditor;
    bool pendingDebtToBank;

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