#include "model/managers/WinConditionManager.hpp"

using namespace std;

WinConditionManager::WinConditionManager() {}

WinConditionManager::WinConditionManager(const WinConditionManager& other) {}

WinConditionManager::~WinConditionManager() {}

WinConditionManager& WinConditionManager::operator=(const WinConditionManager& other) {
    return *this;
}

bool WinConditionManager::isGameOver(const Game& game) const {
    return false;
}

bool WinConditionManager::reachedMaxTurn(const Game& game) const {
    return false;
}

bool WinConditionManager::onlyOnePlayerLeft(const Game& game) const {
    return false;
}

vector<Player*> WinConditionManager::determineWinners(Game& game) const {
    return {};
}