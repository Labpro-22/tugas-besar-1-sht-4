#include "model/managers/FestivalManager.hpp"

using namespace std;

//not used?
FestivalManager::FestivalManager() {}
//not used?
FestivalManager::FestivalManager(const FestivalManager& other) {}
//not used?
FestivalManager::~FestivalManager() {}
//not used?
FestivalManager& FestivalManager::operator=(const FestivalManager& other) {
    return *this;
}

void FestivalManager::activateFestival(StreetTile& tile) {
    tile.activateFestival();
}

void FestivalManager::strengthenFestival(StreetTile& tile) {
    tile.activateFestival();
}

void FestivalManager::decrementFestivalDurations(Player& owner) {
    for (OwnableTile* property : owner.getOwnedProperties()) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr) {
            street->decrementFestivalDuration();
        }
    }
}

void FestivalManager::clearExpiredFestival(StreetTile& tile) {}