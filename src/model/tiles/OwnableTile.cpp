#include "model/tiles/OwnableTile.hpp"

using namespace std;

OwnableTile::OwnableTile()
    : Tile(),
      owner(nullptr),
      ownershipStatus(OwnershipStatus::BANK),
      purchasePrice(0),
      mortgageValue(0)
{}

OwnableTile::OwnableTile(
    int index,
    const string& code,
    const string& name,
    Player* owner,
    OwnershipStatus ownershipStatus,
    int purchasePrice,
    int mortgageValue
) 
    : Tile(index, code, name), 
    owner(owner), 
    ownershipStatus(ownershipStatus), 
    purchasePrice(purchasePrice), 
    mortgageValue(mortgageValue)
{}

OwnableTile::OwnableTile(const OwnableTile& other) 
    : Tile(other), 
    owner(other.owner), 
    ownershipStatus(other.ownershipStatus), 
    purchasePrice(other.purchasePrice), 
    mortgageValue(other.mortgageValue)  {}

OwnableTile::~OwnableTile() {}

OwnableTile& OwnableTile::operator=(const OwnableTile& other) {
    if (this != &other) {
        Tile::operator=(other);
        this->owner = other.owner;
        this->ownershipStatus = other.ownershipStatus;
        this->purchasePrice = other.purchasePrice;
        this->mortgageValue = other.mortgageValue;
    }
    return *this;
}

Player* OwnableTile::getOwner() const {
    return owner;
}

void OwnableTile::setOwner(Player* owner) {
    this->owner = owner;
}

void OwnableTile::setOwnershipStatus(OwnershipStatus ownershipStatus) {
    this->ownershipStatus = ownershipStatus;
}

bool OwnableTile::isOwned() const {
    if (ownershipStatus == OwnershipStatus::OWNED) return true;
    else return false;
}

bool OwnableTile::isMortgaged() const {
    if (ownershipStatus == OwnershipStatus::MORTGAGED) return true;
    else return false;
}

void OwnableTile::mortgage() {
    this->ownershipStatus = OwnershipStatus::MORTGAGED;
}

void OwnableTile::redeem() {
    this->ownershipStatus = OwnershipStatus::OWNED;
}

OwnershipStatus OwnableTile::getOwnershipStatus() const {
    return ownershipStatus;
}

int OwnableTile::getPurchasePrice() const {
    return purchasePrice;
}

int OwnableTile::getMortgageValue() const {
    return mortgageValue;
}