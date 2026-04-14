#include "model/tiles/RailroadTile.hpp"

using namespace std;

RailroadTile::RailroadTile() {}

RailroadTile::RailroadTile(
    int index,
    const string& code,
    const string& name,
    Player* owner,
    OwnershipStatus ownershipStatus,
    int purchasePrice,
    int mortgageValue
)
    : OwnableTile(index, code, name, owner, ownershipStatus, purchasePrice, mortgageValue)
{}

RailroadTile::RailroadTile(const RailroadTile& other)
    : OwnableTile(other)
{}

RailroadTile::~RailroadTile() {}

RailroadTile& RailroadTile::operator=(const RailroadTile& other) {
    if (this != &other) {
        OwnableTile::operator=(other);
    }
    return *this;
}

int RailroadTile::calculateRent(const Game& game, const Player& visitor) const {
    return 0;
}

void RailroadTile::onLand(Game& game, Player& player) {}

void RailroadTile::acquire(Game& game, Player& player) {}

int RailroadTile::getBuildingValue() const {
    return 0;
}

int RailroadTile::getSelltoBankValue() const {
    return purchasePrice;
}