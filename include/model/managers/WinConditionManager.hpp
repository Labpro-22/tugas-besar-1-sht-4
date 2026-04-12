#pragma once

#include <memory>
#include <vector>

using namespace std;

class Game;
class Player;

class WinConditionManager {
public:
    WinConditionManager();
    WinConditionManager(const WinConditionManager& other);
    ~WinConditionManager();
    WinConditionManager& operator=(const WinConditionManager& other);

    bool isGameOver(const Game& game) const;
    bool reachedMaxTurn(const Game& game) const;
    bool onlyOnePlayerLeft(const Game& game) const;
    vector<shared_ptr<Player>> determineWinners(Game& game) const;
};