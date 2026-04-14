#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "model/tiles/OwnableTile.hpp"
#include "model/Game.hpp"
#include "model/NimonException.hpp"

using namespace std;

class Game;
class Player;

class StreetTile : public OwnableTile {
private:
    string colorGroup;
    int houseBuildCost;
    int hotelBuildCost;
    vector<int> rentLevels;
    int buildingLevel;
    int festivalMultiplier;
    int festivalDuration;

public:
    StreetTile();
    StreetTile(
        int index,
        const string& code,
        const string& name,
        Player* owner,
        OwnershipStatus ownershipStatus,
        int purchasePrice,
        int mortgageValue,
        const string& colorGroup,
        int houseBuildCost,
        int hotelBuildCost,
        const vector<int>& rentLevels,
        int buildingLevel,
        int festivalMultiplier,
        int festivalDuration
    );
    StreetTile(const StreetTile& other);
    ~StreetTile();
    StreetTile& operator=(const StreetTile& other);

    void onLand(Game& game, Player& player) override;
    int calculateRent(const Game& game, const Player& visitor) const override;
    bool canBuildHouse(const Game& game) const;
    bool canBuildHotel(const Game& game) const;
    void buildHouse();
    void buildHotel();
    bool hasHotel() const;
    int getBuildingLevel() const;
    string getColorGroup() const;
    void sellBuildings();
    bool isMonopoly(const Game& game) const;
    void activateFestival();
    void decrementFestivalDuration();
    void acquire(Game& game, Player& player) override;

    int getHouseBuildCost() const;
    int getHotelBuildCost() const;
    const vector<int>& getRentLevels() const;
    int getFestivalMultiplier() const;
    int getFestivalDuration() const;
};