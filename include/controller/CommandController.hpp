#pragma once

#include <string>
#include <vector>

using namespace std;

class Game;
class UIManager;
class OwnableTile;
class Player;
class StreetTile;
class Tile;
enum class OwnershipStatus;

class CommandController {
private:
    Game& game;
    UIManager& uiManager;

    string toUpperCopy(string value) const;
    string firstToken(const string& input) const;
    string formatMoney(int amount) const;
    int playerNumberByPointer(const vector<Player>& players, const Player* player) const;
    string ownerLabel(const OwnableTile& tile, const vector<Player>& players) const;
    string ownershipStatusText(OwnershipStatus status) const;
    string normalizeColorKey(string value) const;
    string shortColorCode(const string& colorGroup) const;
    string colorDisplayName(const string& colorGroup) const;
    string categoryCode(const Tile& tile) const;
    string buildingText(const StreetTile& tile) const;
    string buildingMarker(const StreetTile& tile) const;
    string propertyGroupLabel(const OwnableTile& property) const;
    int normalizedPosition(int rawPosition, int boardSize) const;
    string playerTokensForTile(const Game& game, int tileIndex) const;
    void buildDeedData(const OwnableTile& tile, string& title, int& purchasePrice, int& mortgageValue, vector<string>& moneyRowLabels, vector<int>& moneyRowValues, vector<string>& detailRowLabels, vector<string>& detailRowValues, string& ownershipStatus, string& ownerName) const;
    void printDeedFromTile(UIManager& uiManager, const OwnableTile& tile) const;
    int wrappedMove(Game& game, Player& player, int steps) const;
    void fillPropertyOptionVectors(const vector<OwnableTile*>& properties, const string& valueLabel, vector<string>& groups, vector<string>& names, vector<string>& codes, vector<string>& valueLabels, vector<int>& values, vector<string>& statuses, bool useMortgageValue) const;
    bool colorGroupHasBuildings(Game& game, const StreetTile& tile) const;
    int sellColorGroupBuildings(Game& game, Player& player, const StreetTile& tile) const;
    bool handleTripleDoubleJail(Player& player) const;

public:
    CommandController(Game& game, UIManager& uiManager);
    CommandController(const CommandController& other);
    ~CommandController();
    CommandController& operator=(const CommandController& other);

    bool processCommand(const string& input);

    void handleRollDice();
    void handleSetDice(int x, int y);
    void handlePrintBoard();
    void handlePrintDeed();
    void handlePrintProperties();
    void handleCheckMoney();
    void handleMortgage();
    void handleRedeem();
    void handleBuild();
    void handleSave(const string& filename);
    void handlePrintLog(int recentCount = -1);
    void handleUseCard();
    void handleHelp();

    int parseRecentLogCount(const string& input) const;
};
