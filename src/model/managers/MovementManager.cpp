#include "model/managers/MovementManager.hpp"
#include "model/Board.hpp"
#include "model/tiles/Tile.hpp"

using namespace std;

MovementManager::MovementManager() = default;

MovementManager::MovementManager(const MovementManager& other) {}

MovementManager::~MovementManager() {}

MovementManager& MovementManager::operator=(const MovementManager& other) {
    return *this;
}

void MovementManager::movePlayer(Player& player, int steps) {
    player.moveTo(player.getPosition() + steps);
}

void MovementManager::movePlayerTo(Player& player, int targetPosition) {
    player.moveTo(targetPosition);
}

void MovementManager::movePlayerToNearestRailroad(Player& player, const Board& board) {
    const int boardSize = board.getBoardSize();
    const int currentPos = player.getPosition();
    int nearestIndex = -1;
    int minSteps = boardSize + 1;

    for (int i = 1; i <= boardSize; i++) {
        auto tile = board.getTile(i);
        if (tile && tile->onLand() == Tile::TileType::Railroad) {
            int steps = i - currentPos;
            if (steps <= 0) steps += boardSize;
            if (steps < minSteps) {
                minSteps = steps;
                nearestIndex = i;
            }
        }
    }

    if (nearestIndex > 0) {
        player.moveTo(nearestIndex);
    }
}

void MovementManager::movePlayerBack(Player& player, int steps) {
    player.moveTo(player.getPosition() - steps);
}

bool MovementManager::passedGo(const Board& board, int newPos) const {
    int boardSize = board.getBoardSize();
    if (newPos > boardSize) return true;
    return false;
}

void MovementManager::resolveLanding(Player& player) {}