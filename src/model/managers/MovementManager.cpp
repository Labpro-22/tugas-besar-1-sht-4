#include "model/managers/MovementManager.hpp"

using namespace std;

MovementManager::MovementManager() {}

MovementManager::MovementManager(const MovementManager& other) {}

MovementManager::~MovementManager() {}

MovementManager& MovementManager::operator=(const MovementManager& other) {
    return *this;
}

void MovementManager::movePlayer(Game& game, Player& player, int steps) {}

void MovementManager::movePlayerTo(Game& game, Player& player, int targetPosition) {}

void MovementManager::movePlayerToNearestRailroad(Game& game, Player& player) {}

void MovementManager::movePlayerBack(Game& game, Player& player, int steps) {}

bool MovementManager::passedGo(const Board& board, int oldPos, int newPos) const {
    return false;
}

void MovementManager::resolveLanding(Game& game, Player& player) {}