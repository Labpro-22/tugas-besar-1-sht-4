#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "model/tiles/OwnableTile.hpp"
#include "model/NimonException.hpp"

using namespace std;

class Game;
class Player;
class RentContext;

class StreetTile : public OwnableTile {
private:
    string colorGroup;
    int houseBuildCost;
    int hotelBuildCost;
    vector<int> rentLevels;
    int buildingLevel;

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

    TileType onLand() const override;
    int calculateRent(const RentContext& rentContext) const override;
    void buildHouse();
    void buildHotel();
    bool hasHotel() const;
    int getBuildingLevel() const;
    string getColorGroup() const;
    void sellBuildings();
    void setBuildingLevel(int level);
    void acquire(Player& player) override;

    int getHouseBuildCost() const;
    int getHotelBuildCost() const;
    const vector<int>& getRentLevels() const;

    int getBuildingValue() const override;
    int getSelltoBankValue() const override;
};
