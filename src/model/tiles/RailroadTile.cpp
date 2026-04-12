#include "model/tiles/RailroadTile.hpp"

using namespace std;

RailroadTile::RailroadTile() {}

RailroadTile::RailroadTile(
    int index,
    const string& code,
    const string& name,
    shared_ptr<Player> owner,
    OwnershipStatus ownershipStatus,
    int purchasePrice,
    int mortgageValue
) {}

RailroadTile::RailroadTile(const RailroadTile& other) {}

RailroadTile::~RailroadTile() {}

RailroadTile& RailroadTile::operator=(const RailroadTile& other) {
    return *this;
}

int RailroadTile::calculateRent(const Game& game, const Player& visitor) const {
    return 0;
}

void RailroadTile::onLand(Game& game, Player& player) {}

void RailroadTile::acquire(Game& game, Player& player) {}