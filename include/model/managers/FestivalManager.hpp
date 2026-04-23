#pragma once

#include <vector>

using namespace std;

class OwnableTile;

class FestivalManager {
public:
    FestivalManager();
    FestivalManager(const FestivalManager& other);
    ~FestivalManager();
    FestivalManager& operator=(const FestivalManager& other);

    void activateFestival(OwnableTile& tile);
    void strengthenFestival(OwnableTile& tile);
    void decrementFestivalDurations(const vector<OwnableTile*>& ownedProperties);
    void clearExpiredFestival(OwnableTile& tile);
};
