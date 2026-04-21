#pragma once

using namespace std;

#include "model/GameContext.hpp"

class MovementManager {
public:
    MovementManager();
    MovementManager(const MovementManager& other);
    ~MovementManager();
    MovementManager& operator=(const MovementManager& other);

    void movePlayer(Player& player, int steps);
    void movePlayerTo(Player& player, int targetPosition);
    void movePlayerToNearestRailroad(Player& player);
    void movePlayerBack(Player& player, int steps);
    bool passedGo(const Board& board, int newPos) const;
    void resolveLanding(Player& player);
};