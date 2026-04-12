#include "model/tiles/UtilityTile.hpp"

using namespace std;

UtilityTile::UtilityTile() {}

UtilityTile::UtilityTile(
    int index,
    const string& code,
    const string& name,
    shared_ptr<Player> owner,
    OwnershipStatus ownershipStatus,
    int purchasePrice,
    int mortgageValue
) {}

UtilityTile::UtilityTile(const UtilityTile& other) {}

UtilityTile::~UtilityTile() {}

UtilityTile& UtilityTile::operator=(const UtilityTile& other) {
    return *this;
}

int UtilityTile::calculateRent(const Game& game, const Player& visitor) const {
    return 0;
}

void UtilityTile::onLand(Game& game, Player& player) {}

void UtilityTile::acquire(Game& game, Player& player) {}