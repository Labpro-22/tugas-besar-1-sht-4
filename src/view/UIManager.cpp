#include "view/UIManager.hpp"

using namespace std;

UIManager::UIManager() {}

UIManager::UIManager(const UIManager& other) {}

UIManager::~UIManager() {}

UIManager& UIManager::operator=(const UIManager& other) {
    return *this;
}

void UIManager::showMainMenu() const {}

int UIManager::readMainMenuChoice() const {
    return 0;
}

int UIManager::readPlayerCount() const {
    return 0;
}

vector<string> UIManager::readUsernames(int playerCount) const {
    return {};
}

string UIManager::readCommand() const {
    return "";
}

void UIManager::printMessage(const string& msg) const {}

void UIManager::printError(const string& msg) const {}

void UIManager::printBoard(const Game& game) const {}

void UIManager::printDeed(const OwnableTile& tile) const {}

void UIManager::printPlayerProperties(const Player& player) const {}

void UIManager::printLogs(const LogManager& logManager) const {}

void UIManager::printRecentLogs(const LogManager& logManager, int n) const {}

void UIManager::printDiceRoll(int die1, int die2, int total) const {}

void UIManager::printPlayerMovement(const Player& player, int steps, const Tile& destination) const {}

void UIManager::printStreetPurchasePrompt(const Player& player, const StreetTile& tile) const {}

bool UIManager::readYesNo() const {
    return false;
}

void UIManager::printRailroadAcquired(const Player& player, const RailroadTile& tile) const {}

void UIManager::printUtilityAcquired(const Player& player, const UtilityTile& tile) const {}

void UIManager::printRentPayment(const Player& payer, const Player& owner, const OwnableTile& tile, int rent) const {}

void UIManager::printMortgagedNoRent(const OwnableTile& tile, const Player& owner) const {}

void UIManager::printIncomeTaxState(const Player& player, int flatTax, int percentTax) const {}

int UIManager::readIncomeTaxChoice() const {
    return 0;
}

void UIManager::printIncomeTaxBreakdown(const Player& player, int cash, int propertyValue, int buildingValue, int totalWealth, int percent, int taxAmount) const {}

void UIManager::printLuxuryTaxState(const Player& player, int taxAmount) const {}

void UIManager::printFestivalState(const Player& player, const vector<shared_ptr<StreetTile>>& ownedStreets) const {}

string UIManager::readFestivalPropertyCode() const {
    return "";
}

void UIManager::printFestivalActivated(const StreetTile& tile, int oldRent, int newRent, int duration) const {}

void UIManager::printFestivalMaxed(const StreetTile& tile, int duration) const {}

void UIManager::printAuctionState(const StreetTile& tile, int currentBid, shared_ptr<Player> highestBidder, const Player& currentPlayer) const {}

string UIManager::readAuctionAction() const {
    return "";
}

int UIManager::readBidAmount() const {
    return 0;
}

void UIManager::printAuctionWinner(const StreetTile& tile, const Player& winner, int finalBid) const {}

void UIManager::printLiquidationState(const Player& player, int requiredAmount, int estimatedLiquidationValue) const {}

int UIManager::readLiquidationChoice() const {
    return 0;
}

void UIManager::printForceDropState(const Player& player) const {}

int UIManager::readForceDropChoice(int maxIndex) const {
    return 0;
}

void UIManager::printBuildOptions(const Player& player, const vector<string>& eligibleGroups) const {}

int UIManager::readBuildGroupChoice() const {
    return 0;
}

void UIManager::printBuildableTiles(const string& colorGroup, const vector<shared_ptr<StreetTile>>& tiles) const {}

int UIManager::readBuildTileChoice() const {
    return 0;
}

void UIManager::printRedeemOptions(const Player& player, const vector<shared_ptr<OwnableTile>>& redeemableProperties) const {}

int UIManager::readRedeemChoice() const {
    return 0;
}

void UIManager::printMortgageOptions(const Player& player, const vector<shared_ptr<OwnableTile>>& mortgageableProperties) const {}

int UIManager::readMortgageChoice() const {
    return 0;
}

void UIManager::printJailOptions(const Player& player, int jailFine, int failedRolls) const {}

int UIManager::readJailChoice() const {
    return 0;
}

string UIManager::readFilename() const {
    return "";
}

bool UIManager::confirmOverwrite(const string& filename) const {
    return false;
}

void UIManager::printGameOver(const vector<shared_ptr<Player>>& winners, bool byMaxTurn) const {}