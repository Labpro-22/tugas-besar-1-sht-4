#include "model/managers/TurnManager.hpp"

using namespace std;

TurnManager::TurnManager()
    : currentPlayerIndex(0),
      turnOrder(),
      rolledThisTurn(false),
      actionTakenThisTurn(false),
      consecutiveDoubles(0),
      turnCount(0) {}

TurnManager::TurnManager(int currentPlayerIndex, const vector<int>& turnOrder, bool rolledThisTurn, bool actionTakenThisTurn, int consecutiveDoubles, int turnCount) 
: currentPlayerIndex{currentPlayerIndex}, turnOrder{turnOrder}, rolledThisTurn{rolledThisTurn}, actionTakenThisTurn{actionTakenThisTurn}, consecutiveDoubles{consecutiveDoubles}, turnCount{turnCount} {}

TurnManager::TurnManager(const TurnManager& other) {
    this->currentPlayerIndex = other.currentPlayerIndex;
    this->turnOrder = other.turnOrder;
    this->rolledThisTurn = other.rolledThisTurn;
    this->actionTakenThisTurn = other.actionTakenThisTurn;
    this->consecutiveDoubles = other.consecutiveDoubles;
    this->turnCount = other.turnCount;
}

TurnManager::~TurnManager() {}

TurnManager& TurnManager::operator=(const TurnManager& other) {
    if (this != &other) {
        this->currentPlayerIndex = other.currentPlayerIndex;
        this->turnOrder = other.turnOrder;
        this->rolledThisTurn = other.rolledThisTurn;
        this->actionTakenThisTurn = other.actionTakenThisTurn;
        this->consecutiveDoubles = other.consecutiveDoubles;
        this->turnCount = other.turnCount;
    }
    return *this;
}

void TurnManager::initializeTurnOrder(int playerCount) {
    turnOrder.clear();
    for (int i = 0; i < playerCount; i++) {
        turnOrder.push_back(i);
    }
    currentPlayerIndex = 0;
    rolledThisTurn = false;
    actionTakenThisTurn = false;
    consecutiveDoubles = 0;
    turnCount = 1;
}

Player& TurnManager::getCurrentPlayer(GameContext& gameContext) {
    return gameContext.getPlayers().at(turnOrder.at(currentPlayerIndex));  
    // // CHECK (?)
    // throw;
}

void TurnManager::nextPlayer(GameContext& gameContext) {
    if (turnOrder.empty()) {
        initializeTurnOrder(static_cast<int>(gameContext.getPlayers().size()));
    }
    if (turnOrder.empty()) return;
    currentPlayerIndex = (currentPlayerIndex + 1) % static_cast<int>(turnOrder.size());
}

void TurnManager::startTurn(GameContext& gameContext) {
    if (turnOrder.empty()) {
        initializeTurnOrder(static_cast<int>(gameContext.getPlayers().size()));
    }
    rolledThisTurn = false;
    actionTakenThisTurn = false;
    consecutiveDoubles = 0;
}

void TurnManager::endTurn(GameContext& gameContext) {
    if (!turnOrder.empty() && currentPlayerIndex == static_cast<int>(turnOrder.size()) - 1){
        gameContext.setCurrentTurn(gameContext.getCurrentTurn()+1);
        turnCount = gameContext.getCurrentTurn();
    }
    rolledThisTurn = false;
    actionTakenThisTurn = false;
    consecutiveDoubles = 0;
    nextPlayer(gameContext);
}

void TurnManager::registerDiceResult(bool isDouble) {
    rolledThisTurn = true;
    actionTakenThisTurn = true;
    if (isDouble) {
        consecutiveDoubles++;
    } else {
        consecutiveDoubles = 0;
    }
}

void TurnManager::registerAction() {
    actionTakenThisTurn = true;
}

bool TurnManager::canRollDice() const {
    return (!rolledThisTurn);
}

int TurnManager::getCurrentPlayerIndex() const { return currentPlayerIndex; }
const vector<int>& TurnManager::getTurnOrder() const { return turnOrder; }
bool TurnManager::isRolledThisTurn() const { return rolledThisTurn; }
bool TurnManager::hasActionTakenThisTurn() const { return actionTakenThisTurn; }
int TurnManager::getConsecutiveDoubles() const { return consecutiveDoubles; }
int TurnManager::getTurnCount() const { return turnCount; }

void TurnManager::setCurrentPlayerIndex(int idx) { currentPlayerIndex = idx; }
void TurnManager::setTurnOrder(const vector<int>& order) { turnOrder = order; }
void TurnManager::setRolledThisTurn(bool rolled) { rolledThisTurn = rolled; }
void TurnManager::setActionTakenThisTurn(bool taken) { actionTakenThisTurn = taken; }
void TurnManager::setConsecutiveDoubles(int doubles) { consecutiveDoubles = doubles; }
void TurnManager::setTurnCount(int count) { turnCount = count; }

void TurnManager::updateTurnEffects(GameContext& gameContext, Player& player) {

}
