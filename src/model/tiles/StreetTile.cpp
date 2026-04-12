#include "model/tiles/StreetTile.hpp"

using namespace std;

StreetTile::StreetTile() {}

StreetTile::StreetTile(
    int index,
    const string& code,
    const string& name,
    shared_ptr<Player> owner,
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
) {}

StreetTile::StreetTile(const StreetTile& other) {}

StreetTile::~StreetTile() {}

StreetTile& StreetTile::operator=(const StreetTile& other) {
    return *this;
}

void StreetTile::onLand(Game& game, Player& player) {}

int StreetTile::calculateRent(const Game& game, const Player& visitor) const {
    return 0;
}

bool StreetTile::canBuild(const Game& game) const {
    return false;
}

void StreetTile::buildHouse() {}

void StreetTile::buildHotel() {}

bool StreetTile::hasHotel() const {
    return false;
}

int StreetTile::getBuildingLevel() const {
    return 0;
}

void StreetTile::sellBuildings() {}

bool StreetTile::isMonopoly(const Game& game) const {
    return false;
}

void StreetTile::activateFestival() {}

void StreetTile::decrementFestivalDuration() {}

void StreetTile::acquire(Game& game, Player& player) {}