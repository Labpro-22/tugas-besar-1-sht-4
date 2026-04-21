#pragma once

#include <vector>

using namespace std;

class OwnableTile;
class StreetTile;

class FestivalManager {
public:
    FestivalManager();
    FestivalManager(const FestivalManager& other);
    ~FestivalManager();
    FestivalManager& operator=(const FestivalManager& other);

    void activateFestival(StreetTile& tile);
    void strengthenFestival(StreetTile& tile);
    void decrementFestivalDurations(const vector<OwnableTile*>& ownedProperties);
    void clearExpiredFestival(StreetTile& tile);
};