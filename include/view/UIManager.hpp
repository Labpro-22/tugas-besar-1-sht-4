#pragma once

#include <string>
#include <vector>

using namespace std;

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
    string readPropertyCode() const;

    void printBoard(const vector<int>& cellIndices, const vector<string>& cellColorCodes, const vector<vector<string>>& cellLines, int currentTurn, int maxTurn, const string& currentPlayerLabel) const;
    void printDeed(const string& title, int purchasePrice, int mortgageValue, const vector<string>& moneyRowLabels, const vector<int>& moneyRowValues, const vector<string>& detailRowLabels, const vector<string>& detailRowValues, const string& ownershipStatus, const string& ownerName) const;
    void printPlayerProperties(const string& playerName, int playerMoney, int cardCount, const vector<string>& propertyGroups, const vector<string>& propertyNames, const vector<string>& propertyCodes, const vector<string>& propertyBuildingStatuses, const vector<string>& propertyStatuses, const vector<int>& propertyValues, int totalPropertyWealth) const;
    void printLogs(const vector<string>& formattedLogs) const;
    void printRecentLogs(const vector<string>& formattedLogs, int n) const;
    void printDiceRoll(int die1, int die2, int total) const;
    void printPlayerMovement(const string& playerName, int steps, const string& destinationName, const string& destinationCode) const;
    void printStreetPurchasePrompt(const string& playerName, int playerMoney, const string& title, int purchasePrice, int mortgageValue, const vector<string>& moneyRowLabels, const vector<int>& moneyRowValues, const vector<string>& detailRowLabels, const vector<string>& detailRowValues, const string& ownershipStatus, const string& ownerName) const;
    bool readYesNo() const;
    void printRailroadAcquired(const string& playerName, const string& tileName, const string& tileCode) const;
    void printUtilityAcquired(const string& playerName, const string& tileName, const string& tileCode) const;
    void printRentPayment(const string& payerName, int payerMoney, const string& ownerName, int ownerMoney, const string& tileName, const string& tileCode, const string& condition, const string& festivalStatus, int rent) const;
    void printMortgagedNoRent(const string& tileName, const string& tileCode, const string& ownerName) const;
    void printIncomeTaxState(const string& playerName, int playerMoney, int flatTax, int percentTax) const;
    int readIncomeTaxChoice() const;
    void printIncomeTaxBreakdown(int currentMoney, int cash, int propertyValue, int buildingValue, int totalWealth, int percent, int taxAmount) const;
    void printLuxuryTaxState(int playerMoney, int taxAmount) const;
    void printFestivalState(const string& playerName, const vector<string>& propertyNames, const vector<string>& propertyCodes, const vector<string>& propertyStatuses) const;
    string readFestivalPropertyCode() const;
    void printFestivalActivated(const string& tileName, const string& tileCode, int oldRent, int newRent, int duration) const;
    void printFestivalMaxed(const string& tileName, const string& tileCode, int duration) const;
    void printAuctionState(const string& tileName, const string& tileCode, int currentBid, const string& highestBidderName, const string& currentPlayerName) const;
    string readAuctionAction() const;
    void printAuctionWinner(const string& tileName, const string& tileCode, const string& winnerName, int finalBid) const;
    void printLiquidationState(const string& playerName, int playerMoney, int requiredAmount, int estimatedLiquidationValue) const;
    int readLiquidationChoice() const;
    void printForceDropState(const string& playerName, const vector<string>& cardNames) const;
    int readForceDropChoice(int maxIndex) const;
    void printAbilityCardOptions(const vector<string>& cardNames, const vector<string>& cardDescriptions) const;
    int readAbilityCardChoice(int maxIndex) const;
    void printBuildOptions(int playerMoney, const vector<string>& eligibleGroups) const;
    int readBuildGroupChoice() const;
    void printBuildableTiles(const string& colorGroup, const vector<string>& tileNames, const vector<string>& tileCodes, const vector<string>& buildingStatuses, const vector<string>& buildStatuses) const;
    int readBuildTileChoice() const;
    void printRedeemOptions(int playerMoney, const vector<string>& propertyGroups, const vector<string>& propertyNames, const vector<string>& propertyCodes, const vector<string>& valueLabels, const vector<int>& values, const vector<string>& propertyStatuses) const;
    int readRedeemChoice() const;
    void printMortgageOptions(int playerMoney, const vector<string>& propertyGroups, const vector<string>& propertyNames, const vector<string>& propertyCodes, const vector<string>& valueLabels, const vector<int>& values, const vector<string>& propertyStatuses) const;
    int readMortgageChoice() const;
    void printJailOptions(const string& playerName, int playerMoney, int jailFine, int failedRolls) const;
    int readJailChoice() const;
    string readFilename() const;
    bool confirmOverwrite(const string& filename) const;
    void printGameOver(const vector<string>& winnerNames, const vector<int>& winnerMoney, const vector<int>& winnerPropertyCounts, const vector<int>& winnerCardCounts, bool byMaxTurn) const;
};