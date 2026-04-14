#include "controller/GameController.hpp"

#include "model/Game.hpp"
#include "view/UIManager.hpp"

using namespace std;

GameController::GameController(Game& game, UIManager& uiManager)
    : game(game), uiManager(uiManager) {}

GameController::GameController(const GameController& other)
    : game(other.game), uiManager(other.uiManager) {}

GameController::~GameController() {}

GameController& GameController::operator=(const GameController& other) {
    return *this;
}

void GameController::processCommand(const string& input) {}

void GameController::handleRollDice() {}

void GameController::handleSetDice(int x, int y) {}

void GameController::handlePrintBoard() {}

void GameController::handlePrintDeed() {}

void GameController::handlePrintProperties() {}

void GameController::handleMortgage() {}

void GameController::handleRedeem() {}

void GameController::handleBuild() {}

void GameController::handleSave(const string& filename) {}

void GameController::handlePrintLog(int recentCount) {}

void GameController::handleUseCard() {}

void GameController::handleStartTurn() {}

void GameController::handleEndTurn() {}

void GameController::handleStreetPurchase(StreetTile& tile) {}

void GameController::handleAuction(StreetTile& tile, Player* triggerPlayer) {}

void GameController::handleIncomeTax(IncomeTaxTile& tile) {}

void GameController::handleLuxuryTax(LuxuryTaxTile& tile) {}

void GameController::handleFestival(FestivalTile& tile) {}

void GameController::handleForceDrop(Player& player) {}

void GameController::handleJailTurn(Player& player) {}

void GameController::handleBankruptcy(Player& player) {}

bool GameController::canSaveNow() const {
    return false;
}

bool GameController::isCommandValidThisTurn(const string& input) const {
    return false;
}

int GameController::parseRecentLogCount(const string& input) const {
    return -1;
}

void GameController::runGameLoop() {}

void GameController::runTurn() {}