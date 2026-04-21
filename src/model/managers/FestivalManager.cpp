#include "model/managers/FestivalManager.hpp"

#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/StreetTile.hpp"

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

void FestivalManager::decrementFestivalDurations(const vector<OwnableTile*>& ownedProperties) {
    for (OwnableTile* property : ownedProperties) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr) {
            street->decrementFestivalDuration();
        }
    }
}

void FestivalManager::clearExpiredFestival(StreetTile& tile) {}