#include "model/managers/TurnManager.hpp"

using namespace std;

TurnManager::TurnManager() {}

TurnManager::TurnManager(int currentPlayerIndex, const vector<int>& turnOrder, bool rolledThisTurn, int consecutiveDoubles, int turnCount) {}

TurnManager::TurnManager(const TurnManager& other) {}

TurnManager::~TurnManager() {}

TurnManager& TurnManager::operator=(const TurnManager& other) {
    return *this;
}

void TurnManager::initializeTurnOrder(int playerCount) {}

Player& TurnManager::getCurrentPlayer(Game& game) {
    throw;
}

void TurnManager::nextPlayer(Game& game) {}

void TurnManager::startTurn(Game& game) {}

void TurnManager::endTurn(Game& game) {}

void TurnManager::registerDiceResult(bool isDouble) {}

bool TurnManager::canRollDice() const {
    return false;
}

void TurnManager::updateTurnEffects(Game& game, Player& player) {}