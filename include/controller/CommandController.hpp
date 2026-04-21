#pragma once

#include <string>

using namespace std;

class Game;
class UIManager;

class CommandController {
private:
    Game& game;
    UIManager& uiManager;

public:
    CommandController(Game& game, UIManager& uiManager);
    CommandController(const CommandController& other);
    ~CommandController();
    CommandController& operator=(const CommandController& other);

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

    int parseRecentLogCount(const string& input) const;
};