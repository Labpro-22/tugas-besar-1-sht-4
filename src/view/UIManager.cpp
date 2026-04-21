#include "view/UIManager.hpp"

#include <iostream>
#include <limits>

using namespace std;

UIManager::UIManager() {}

UIManager::UIManager(const UIManager& other) {}

UIManager::~UIManager() {}

UIManager& UIManager::operator=(const UIManager& other) {
    if (this != &other) {
    }
    return *this;
}

void UIManager::showMainMenu() const {
    cout << "=================================\n";
    cout << "        NIMONSPOLI CLI\n";
    cout << "=================================\n";
    cout << "1. New Game\n";
    cout << "2. Load Game\n";
    cout << "3. Exit\n";
    cout << "=================================\n";
}

int UIManager::readMainMenuChoice() const {
    int choice;

    while (true) {
        cout << "Pilih menu (1-3): ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input harus berupa angka.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice >= 1 && choice <= 3) {
            return choice;
        }

        cout << "Pilihan hanya boleh 1 sampai 3.\n";
    }
}

int UIManager::readPlayerCount() const {
    int count;

    while (true) {
        cout << "Masukkan jumlah pemain (2-4): ";
        cin >> count;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input harus berupa angka.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (count >= 2 && count <= 4) {
            return count;
        }

        cout << "Jumlah pemain harus antara 2 sampai 4.\n";
    }
}

vector<string> UIManager::readUsernames(int playerCount) const {
    vector<string> usernames;

    for (int i = 0; i < playerCount; i++) {
        string username;

        while (true) {
            cout << "Masukkan username pemain " << (i + 1) << ": ";
            getline(cin, username);

            if (!username.empty()) {
                usernames.push_back(username);
                break;
            }

            cout << "Username tidak boleh kosong.\n";
        }
    }

    return usernames;
}

string UIManager::readCommand() const {
    string command;
    cout << "> ";
    getline(cin, command);
    return command;
}

void UIManager::printMessage(const string& msg) const {
    cout << msg << '\n';
}

void UIManager::printError(const string& msg) const {
    cerr << "Error: " << msg << '\n';
}

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

void UIManager::printAuctionState(const StreetTile& tile, int currentBid, Player* highestBidder, const Player& currentPlayer) const {}

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

void UIManager::printBuildableTiles(const string& colorGroup, const vector<StreetTile*>& tiles) const {}

int UIManager::readBuildTileChoice() const {
    return 0;
}

void UIManager::printRedeemOptions(const Player& player, const vector<OwnableTile*>& redeemableProperties) const {}

int UIManager::readRedeemChoice() const {
    return 0;
}

void UIManager::printMortgageOptions(const Player& player, const vector<OwnableTile*>& mortgageableProperties) const {}

int UIManager::readMortgageChoice() const {
    return 0;
}

void UIManager::printJailOptions(const Player& player, int jailFine, int failedRolls) const {}

int UIManager::readJailChoice() const {
    return 0;
}

string UIManager::readFilename() const {
    string filename;

    while (true) {
        cout << "Masukkan nama file: ";
        getline(cin, filename);

        if (!filename.empty()) {
            return filename;
        }

        cout << "Nama file tidak boleh kosong.\n";
    }
}

bool UIManager::confirmOverwrite(const string& filename) const {
    return false;
}

void UIManager::printGameOver(const vector<Player*>& winners, bool byMaxTurn) const {}