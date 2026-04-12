#include "model/tiles/OwnableTile.hpp"

using namespace std;

OwnableTile::OwnableTile() {}

OwnableTile::OwnableTile(
    int index,
    const string& code,
    const string& name,
    shared_ptr<Player> owner,
    OwnershipStatus ownershipStatus,
    int purchasePrice,
    int mortgageValue
) {}

OwnableTile::OwnableTile(const OwnableTile& other) {}

OwnableTile::~OwnableTile() {}

OwnableTile& OwnableTile::operator=(const OwnableTile& other) {
    return *this;
}

shared_ptr<Player> OwnableTile::getOwner() const {
    return owner;
}

void OwnableTile::setOwner(shared_ptr<Player> owner) {}

bool OwnableTile::isOwned() const {
    return false;
}

bool OwnableTile::isMortgaged() const {
    return false;
}

void OwnableTile::mortgage() {}

void OwnableTile::redeem() {}