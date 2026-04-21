#include "model/managers/WinConditionManager.hpp"

#include "model/Game.hpp"
#include "model/Player.hpp"

using namespace std;

// not used?
WinConditionManager::WinConditionManager() {}
// not used?
WinConditionManager::WinConditionManager(const WinConditionManager& other) {}
// not used?
WinConditionManager::~WinConditionManager() {}
// not used?
WinConditionManager& WinConditionManager::operator=(const WinConditionManager& other) {
    return *this;
}

bool WinConditionManager::isGameOver(const Game& game) const {
    if (onlyOnePlayerLeft(game)) {
        return true;
    }
    // cek turn belum ditambahin
    // if (game.getTurn() >= game.getMaxTurns()) {
    return true;
}

bool WinConditionManager::reachedMaxTurn(const Game& game) const {
    return game.getCurrentTurn() >= game.getMaxTurn();
}

bool WinConditionManager::onlyOnePlayerLeft(const Game& game) const {
    int remainingPlayers = 0;
    for (const Player& player : game.getPlayers()) {
        if (!player.isBankrupt()) {
            remainingPlayers++;
        }
    }
    return remainingPlayers == 1;
}

vector<Player*> WinConditionManager::determineWinners(Game& game) const {
    vector<Player*> winners;
    for (const Player& player : game.getPlayers()) {
        if (!player.isBankrupt()) {
            winners.push_back(const_cast<Player*>(&player));
        }
    }
    return winners;
}
