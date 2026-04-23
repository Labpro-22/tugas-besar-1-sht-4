#include "model/tiles/OwnableTile.hpp"

#include <algorithm>

using namespace std;

OwnableTile::OwnableTile()
    : Tile(),
      owner(nullptr),
      ownershipStatus(OwnershipStatus::BANK),
      purchasePrice(0),
      mortgageValue(0),
      festivalMultiplier(1),
      festivalDuration(0)
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
    mortgageValue(mortgageValue),
    festivalMultiplier(1),
    festivalDuration(0)
{}

OwnableTile::OwnableTile(const OwnableTile& other) 
    : Tile(other), 
    owner(other.owner), 
    ownershipStatus(other.ownershipStatus), 
    purchasePrice(other.purchasePrice), 
    mortgageValue(other.mortgageValue),
    festivalMultiplier(other.festivalMultiplier),
    festivalDuration(other.festivalDuration)  {}

OwnableTile::~OwnableTile() {}

OwnableTile& OwnableTile::operator=(const OwnableTile& other) {
    if (this != &other) {
        Tile::operator=(other);
        this->owner = other.owner;
        this->ownershipStatus = other.ownershipStatus;
        this->purchasePrice = other.purchasePrice;
        this->mortgageValue = other.mortgageValue;
        this->festivalMultiplier = other.festivalMultiplier;
        this->festivalDuration = other.festivalDuration;
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

int OwnableTile::getFestivalMultiplier() const {
    return festivalMultiplier;
}

int OwnableTile::getFestivalDuration() const {
    return festivalDuration;
}

void OwnableTile::activateFestival() {
    if (festivalMultiplier < 2) festivalMultiplier = 2;
    else if (festivalMultiplier < 8) festivalMultiplier *= 2;
    festivalDuration = 3;
}

void OwnableTile::decrementFestivalDuration() {
    if (festivalDuration > 0) {
        festivalDuration--;
    }
    if (festivalDuration == 0) {
        festivalMultiplier = 1;
    }
}

void OwnableTile::setFestivalState(int multiplier, int duration) {
    festivalMultiplier = max(1, multiplier);
    festivalDuration = max(0, duration);
    if (festivalDuration == 0) {
        festivalMultiplier = 1;
    }
}
