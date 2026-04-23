#include "model/managers/FestivalManager.hpp"

#include "model/tiles/OwnableTile.hpp"

using namespace std;

//not used?
FestivalManager::FestivalManager() = default;
//not used?
FestivalManager::FestivalManager(const FestivalManager& other) {
    (void) other;
}
//not used?
FestivalManager::~FestivalManager() {}
//not used?
FestivalManager& FestivalManager::operator=(const FestivalManager& other) {
    (void) other;
    return *this;
}

void FestivalManager::activateFestival(OwnableTile& tile) {
    tile.activateFestival();
}

void FestivalManager::strengthenFestival(OwnableTile& tile) {
    tile.activateFestival();
}

void FestivalManager::decrementFestivalDurations(const vector<OwnableTile*>& ownedProperties) {
    for (OwnableTile* property : ownedProperties) {
        if (property != nullptr) {
            property->decrementFestivalDuration();
        }
    }
}

void FestivalManager::clearExpiredFestival(OwnableTile& tile) {
    if (tile.getFestivalDuration() <= 0) {
        tile.setFestivalState(1, 0);
    }
}
