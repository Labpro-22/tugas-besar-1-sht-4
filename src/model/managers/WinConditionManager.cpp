#include "model/managers/WinConditionManager.hpp"

#include "model/Game.hpp"
#include "model/Player.hpp"

using namespace std;

// not used?
WinConditionManager::WinConditionManager() = default;
// not used?
WinConditionManager::WinConditionManager(const WinConditionManager& other) {
    (void) other;
}
// not used?
WinConditionManager::~WinConditionManager() {}
// not used?
WinConditionManager& WinConditionManager::operator=(const WinConditionManager& other) {
    (void) other;
    return *this;
}

bool WinConditionManager::isGameOver(const Game& game) const {
    if (onlyOnePlayerLeft(game)) {
        return true;
    }
    return reachedMaxTurn(game);
}

bool WinConditionManager::reachedMaxTurn(const Game& game) const {
    return game.getCurrentTurn() > game.getMaxTurn();
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
    int bestMoney = 0;
    int bestProperties = 0;
    int bestCards = 0;
    bool hasCandidate = false;

    for (Player& player : game.getPlayers()) {
        if (player.isBankrupt()) {
            continue;
        }

        const int money = player.getMoney();
        const int properties = game.getPropertyManager().countOwnedProperties(game.getBoard(), player);
        const int cards = player.countCards();
        const bool better = !hasCandidate ||
                            money > bestMoney ||
                            (money == bestMoney && properties > bestProperties) ||
                            (money == bestMoney && properties == bestProperties && cards > bestCards);

        if (better) {
            winners.clear();
            bestMoney = money;
            bestProperties = properties;
            bestCards = cards;
            hasCandidate = true;
        }

        if (money == bestMoney && properties == bestProperties && cards == bestCards) {
            winners.push_back(&player);
        }
    }
    return winners;
}
