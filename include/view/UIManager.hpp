#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class Game;
class Player;
class Tile;
class OwnableTile;
class StreetTile;
class RailroadTile;
class UtilityTile;
class LogManager;

class UIManager {
public:
    UIManager();
    UIManager(const UIManager& other);
    ~UIManager();
    UIManager& operator=(const UIManager& other);

    void showMainMenu() const;
    int readMainMenuChoice() const;
    int readPlayerCount() const;
    vector<string> readUsernames(int playerCount) const;
    string readCommand() const;
    void printMessage(const string& msg) const;
    void printError(const string& msg) const;
    void printBoard(const Game& game) const;
    void printDeed(const OwnableTile& tile) const;
    void printPlayerProperties(const Player& player) const;
    void printLogs(const LogManager& logManager) const;
    void printRecentLogs(const LogManager& logManager, int n) const;
    void printDiceRoll(int die1, int die2, int total) const;
    void printPlayerMovement(const Player& player, int steps, const Tile& destination) const;
    void printStreetPurchasePrompt(const Player& player, const StreetTile& tile) const;
    bool readYesNo() const;
    void printRailroadAcquired(const Player& player, const RailroadTile& tile) const;
    void printUtilityAcquired(const Player& player, const UtilityTile& tile) const;
    void printRentPayment(const Player& payer, const Player& owner, const OwnableTile& tile, int rent) const;
    void printMortgagedNoRent(const OwnableTile& tile, const Player& owner) const;
    void printIncomeTaxState(const Player& player, int flatTax, int percentTax) const;
    int readIncomeTaxChoice() const;
    void printIncomeTaxBreakdown(const Player& player, int cash, int propertyValue, int buildingValue, int totalWealth, int percent, int taxAmount) const;
    void printLuxuryTaxState(const Player& player, int taxAmount) const;
    void printFestivalState(const Player& player, const vector<shared_ptr<StreetTile>>& ownedStreets) const;
    string readFestivalPropertyCode() const;
    void printFestivalActivated(const StreetTile& tile, int oldRent, int newRent, int duration) const;
    void printFestivalMaxed(const StreetTile& tile, int duration) const;
    void printAuctionState(const StreetTile& tile, int currentBid, Player* highestBidder, const Player& currentPlayer) const;
    string readAuctionAction() const;
    int readBidAmount() const;
    void printAuctionWinner(const StreetTile& tile, const Player& winner, int finalBid) const;
    void printLiquidationState(const Player& player, int requiredAmount, int estimatedLiquidationValue) const;
    int readLiquidationChoice() const;
    void printForceDropState(const Player& player) const;
    int readForceDropChoice(int maxIndex) const;
    void printBuildOptions(const Player& player, const vector<string>& eligibleGroups) const;
    int readBuildGroupChoice() const;
    void printBuildableTiles(const string& colorGroup, const vector<shared_ptr<StreetTile>>& tiles) const;
    int readBuildTileChoice() const;
    void printRedeemOptions(const Player& player, const vector<shared_ptr<OwnableTile>>& redeemableProperties) const;
    int readRedeemChoice() const;
    void printMortgageOptions(const Player& player, const vector<shared_ptr<OwnableTile>>& mortgageableProperties) const;
    int readMortgageChoice() const;
    void printJailOptions(const Player& player, int jailFine, int failedRolls) const;
    int readJailChoice() const;
    string readFilename() const;
    bool confirmOverwrite(const string& filename) const;
    void printGameOver(const vector<Player*>& winners, bool byMaxTurn) const;
};