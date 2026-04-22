#include "model/managers/MovementManager.hpp"

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

void MovementManager::movePlayerToNearestRailroad(Player& player) {

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