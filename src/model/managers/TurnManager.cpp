#include "model/managers/TurnManager.hpp"

using namespace std;

TurnManager::TurnManager() {}

TurnManager::TurnManager(int currentPlayerIndex, const vector<int>& turnOrder, bool rolledThisTurn, int consecutiveDoubles, int turnCount) 
: currentPlayerIndex{currentPlayerIndex}, turnOrder{turnOrder}, rolledThisTurn{rolledThisTurn}, consecutiveDoubles{consecutiveDoubles}, turnCount{turnCount} {}

TurnManager::TurnManager(const TurnManager& other) {
    this->currentPlayerIndex = other.currentPlayerIndex;
    this->turnOrder = other.turnOrder;
    this->rolledThisTurn = other.rolledThisTurn;
    this->consecutiveDoubles = other.consecutiveDoubles;
    this->turnCount = other.turnCount;
}

TurnManager::~TurnManager() {}

TurnManager& TurnManager::operator=(const TurnManager& other) {
    if (this != &other) {
        this->currentPlayerIndex = other.currentPlayerIndex;
        this->turnOrder = other.turnOrder;
        this->rolledThisTurn = other.rolledThisTurn;
        this->consecutiveDoubles = other.consecutiveDoubles;
        this->turnCount = other.turnCount;
    }
    return *this;
}

void TurnManager::initializeTurnOrder(int playerCount) {

}

Player& TurnManager::getCurrentPlayer(Game& game) {
    return game.getPlayers().at(turnOrder.at(currentPlayerIndex));  
    // // CHECK (?)
    // throw;
}

void TurnManager::nextPlayer(Game& game) {

}

void TurnManager::startTurn(Game& game) {}

void TurnManager::endTurn(Game& game) {
    rolledThisTurn = true;
    consecutiveDoubles = 0;

}

void TurnManager::registerDiceResult(bool isDouble) {
    rolledThisTurn = true;
    if (isDouble) {
        consecutiveDoubles++;
    } else {
        consecutiveDoubles = 0;
    }
}

bool TurnManager::canRollDice() const {
    return (!rolledThisTurn);
}

void TurnManager::updateTurnEffects(Game& game, Player& player) {

}