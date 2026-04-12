#pragma once

using namespace std;

class Game;
class Player;
class Board;

class MovementManager {
public:
    MovementManager();
    MovementManager(const MovementManager& other);
    ~MovementManager();
    MovementManager& operator=(const MovementManager& other);

    void movePlayer(Game& game, Player& player, int steps);
    void movePlayerTo(Game& game, Player& player, int targetPosition);
    void movePlayerToNearestRailroad(Game& game, Player& player);
    void movePlayerBack(Game& game, Player& player, int steps);
    bool passedGo(const Board& board, int oldPos, int newPos) const;
    void resolveLanding(Game& game, Player& player);
};