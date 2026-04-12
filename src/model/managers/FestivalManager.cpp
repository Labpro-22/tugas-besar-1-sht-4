#include "model/managers/FestivalManager.hpp"

using namespace std;

FestivalManager::FestivalManager() {}

FestivalManager::FestivalManager(const FestivalManager& other) {}

FestivalManager::~FestivalManager() {}

FestivalManager& FestivalManager::operator=(const FestivalManager& other) {
    return *this;
}

void FestivalManager::activateFestival(Game& game, Player& player, StreetTile& tile) {}

void FestivalManager::strengthenFestival(StreetTile& tile) {}

void FestivalManager::decrementFestivalDurations(Player& owner) {}

void FestivalManager::clearExpiredFestival(StreetTile& tile) {}