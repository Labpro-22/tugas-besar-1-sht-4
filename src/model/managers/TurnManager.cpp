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
    vector<int> order(playerCount);
    for (int i = 0; i < playerCount; i++) {
        order[i] = i;
    }
}

Player& TurnManager::getCurrentPlayer(GameContext& gameContext) {
    return gameContext.getPlayers().at(turnOrder.at(currentPlayerIndex));  
    // // CHECK (?)
    // throw;
}

void TurnManager::nextPlayer(GameContext& gameContext) {
    // initialize next player. Tergantung turnCount
}

void TurnManager::startTurn(GameContext& gameContext) {}

void TurnManager::endTurn(GameContext& gameContext) {
    if (currentPlayerIndex == turnOrder.back()){
        gameContext.setCurrentTurn(gameContext.getCurrentTurn()+1);
    }
    rolledThisTurn = true;
    consecutiveDoubles = 0;
    nextPlayer(gameContext);
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

void TurnManager::updateTurnEffects(GameContext& gameContext, Player& player) {

}