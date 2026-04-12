#pragma once

#include <memory>
#include <string>

using namespace std;

class Game;
class UIManager;
class Player;
class StreetTile;
class IncomeTaxTile;
class LuxuryTaxTile;
class FestivalTile;

class GameController {
private:
    Game& game;
    UIManager& uiManager;

public:
    GameController(Game& game, UIManager& uiManager);
    GameController(const GameController& other);
    ~GameController();
    GameController& operator=(const GameController& other);

    void processCommand(const string& input);
    void handleRollDice();
    void handleSetDice(int x, int y);
    void handlePrintBoard();
    void handlePrintDeed();
    void handlePrintProperties();
    void handleMortgage();
    void handleRedeem();
    void handleBuild();
    void handleSave(const string& filename);
    void handlePrintLog(int recentCount = -1);
    void handleUseCard();
    void handleStartTurn();
    void handleEndTurn();
    void handleStreetPurchase(StreetTile& tile);
    void handleAuction(StreetTile& tile, shared_ptr<Player> triggerPlayer);
    void handleIncomeTax(IncomeTaxTile& tile);
    void handleLuxuryTax(LuxuryTaxTile& tile);
    void handleFestival(FestivalTile& tile);
    void handleForceDrop(Player& player);
    void handleJailTurn(Player& player);
    void handleBankruptcy(Player& player);
    bool canSaveNow() const;
    bool isCommandValidThisTurn(const string& input) const;
    int parseRecentLogCount(const string& input) const;
    void runGameLoop();
    void runTurn();
};