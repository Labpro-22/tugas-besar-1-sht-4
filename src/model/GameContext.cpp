#include "model/GameContext.hpp"

GameContext::GameContext() : currentTurn(0), maxTurn(100), isRunning(false), board(), players() {}

GameContext::GameContext(int currentTurn, int maxTurn, bool isRunning)
    : currentTurn(currentTurn), maxTurn(maxTurn), isRunning(isRunning), board(), players() {}

GameContext::GameContext(const GameContext& other)
    : currentTurn(other.currentTurn),
      maxTurn(other.maxTurn),
      isRunning(other.isRunning),
      board(other.board),
      players(other.players) {}

GameContext::~GameContext() {}

GameContext& GameContext::operator=(const GameContext& other) {
    if (this != &other) {
        currentTurn = other.currentTurn;
        maxTurn = other.maxTurn;
        isRunning = other.isRunning;
        board = other.board;
        players = other.players;
    }
    return *this;
}

int GameContext::getCurrentTurn() const {
    return currentTurn;
}

int GameContext::getMaxTurn() const {
    return maxTurn;
}

bool GameContext::isGameRunning() const {
    return isRunning;
}

Board& GameContext::getBoard() {
    return board;
}

const Board& GameContext::getBoard() const {
    return board;
}

std::vector<Player>& GameContext::getPlayers() {
    return players;
}

const std::vector<Player>& GameContext::getPlayers() const {
    return players;
}

void GameContext::setCurrentTurn(int currentTurn) {
    this->currentTurn = currentTurn;
}

void GameContext::setMaxTurn(int maxTurn) {
    this->maxTurn = maxTurn;
}

void GameContext::setIsRunning(bool isRunning) {
    this->isRunning = isRunning;
}
