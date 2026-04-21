#pragma once

using namespace std;

#include "model/GameContext.hpp"
#include "model/tiles/StreetTile.hpp"

class FestivalManager {
public:
    FestivalManager();
    FestivalManager(const FestivalManager& other);
    ~FestivalManager();
    FestivalManager& operator=(const FestivalManager& other);

    void activateFestival(StreetTile& tile);
    void strengthenFestival(StreetTile& tile);
    void decrementFestivalDurations(Player& owner);
    void clearExpiredFestival(StreetTile& tile);
};