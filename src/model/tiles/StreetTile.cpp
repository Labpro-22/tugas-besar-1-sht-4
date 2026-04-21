#include "model/tiles/StreetTile.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

using namespace std;

StreetTile::StreetTile() {}

StreetTile::StreetTile(
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
)  
    : OwnableTile(index, code, name, owner, ownershipStatus, purchasePrice, mortgageValue), 
    colorGroup(colorGroup), 
    houseBuildCost(houseBuildCost),
    hotelBuildCost(hotelBuildCost),
    rentLevels(rentLevels),
    buildingLevel(buildingLevel),
    festivalMultiplier(festivalMultiplier),
    festivalDuration(festivalDuration)
{}

StreetTile::StreetTile(const StreetTile& other) 
    : OwnableTile(other), 
    colorGroup(other.colorGroup), 
    houseBuildCost(other.houseBuildCost), 
    hotelBuildCost(other.hotelBuildCost), 
    rentLevels(other.rentLevels), 
    buildingLevel(other.buildingLevel), 
    festivalMultiplier(other.festivalMultiplier), 
    festivalDuration(other.festivalDuration)
{}

StreetTile::~StreetTile() {}

StreetTile& StreetTile::operator=(const StreetTile& other) {
    if (this != &other) {
        OwnableTile::operator=(other);
        this->colorGroup = other.colorGroup;
        this->houseBuildCost = other.houseBuildCost;
        this->hotelBuildCost = other.hotelBuildCost;
        this->rentLevels = other.rentLevels;
        this->buildingLevel = other.buildingLevel;
        this->festivalMultiplier = other.festivalMultiplier;
        this->festivalDuration = other.festivalDuration;
    }
    return *this;
}

void StreetTile::onLand(Game& game, Player& player) {
    // TODO : implement ui maybe
}

// TODO : edit the game object to include constant methods
int StreetTile::calculateRent(const Game& game, const Player& visitor) const {
    // TODO : make sure post build rent rules 
    if (ownershipStatus == OwnershipStatus::MORTGAGED) return 0;
    int rent = 0;
    if (buildingLevel == 0) {
        rent = rentLevels[0];
        if (isMonopoly(game)) rent *= 2;
    }
    else rent = rentLevels[buildingLevel];

    rent *= festivalMultiplier;
    return rent;
}

// TODO : change back into const function and const game, after adding game const board getter
bool StreetTile::canBuildHouse(const Game& game) const {
    // check if owned
    if (this->getOwner() == nullptr) return false;

    // check if owned all colors
    vector<shared_ptr<StreetTile>> temp = game.getBoard().getStreetTileByColorGroup(this->colorGroup);

    if (temp.empty()) return false;

    if (!all_of(temp.begin(), temp.end(), [&](const shared_ptr<StreetTile>& streetTile) {
        return streetTile->getOwner() == this->getOwner();
    })) return false;

    // check if already hotel
    if (this->hasHotel()) return false;

    // check if distribution is even 
    int minBuildingLevel = min_element(temp.begin(), temp.end(), [](const shared_ptr<StreetTile>& streetTile1, const shared_ptr<StreetTile>& streetTile2) {
        return streetTile1->getBuildingLevel() < streetTile2->getBuildingLevel();
    })->get()->getBuildingLevel();

    if (this->buildingLevel != minBuildingLevel) return false;

    // check if enough money
    if (this->getOwner()->getMoney() < houseBuildCost) return false;

    // check if already 4
    if (this->getBuildingLevel() == 4) return false;

    return true;
}

bool StreetTile::canBuildHotel(const Game& game) const {
    // check if owned
    if (this->getOwner() == nullptr) return false;

    // check if owned all colors
    vector<shared_ptr<StreetTile>> temp = game.getBoard().getStreetTileByColorGroup(this->colorGroup);

    if (temp.empty()) return false;

    if (!all_of(temp.begin(), temp.end(), [&](const shared_ptr<StreetTile>& streetTile) {
        return streetTile->getOwner() == this->getOwner();
    })) return false;

    // check if already hotel
    if (this->hasHotel()) return false;

    // check if distribution is even 
    int minBuildingLevel = min_element(temp.begin(), temp.end(), [](const shared_ptr<StreetTile>& streetTile1, const shared_ptr<StreetTile>& streetTile2) {
        return streetTile1->getBuildingLevel() < streetTile2->getBuildingLevel();
    })->get()->getBuildingLevel();

    if (minBuildingLevel != 4) return false;
    if (this->getBuildingLevel() != 4) return false;

    // check if enough money
    if (this->getOwner()->getMoney() < hotelBuildCost) return false;

    return true;
}

// no error checking, check yourself
void StreetTile::buildHouse() {
    buildingLevel++;
}

// no error checking, check yourself
void StreetTile::buildHotel() {
    buildingLevel = 5;
}

bool StreetTile::hasHotel() const {
    return this->buildingLevel == 5;
}

int StreetTile::getBuildingLevel() const {
    return this->buildingLevel;
}

void StreetTile::sellBuildings() {
    // TODO : make sure of what's asked, then edit this
    this->buildingLevel == 0;
}

// TODO : after adding const function into game, edit this back into const game and function
bool StreetTile::isMonopoly(const Game& game) const {
    if (this->getOwner() == nullptr) return false;
    vector<shared_ptr<StreetTile>> temp = game.getBoard().getStreetTileByColorGroup(this->colorGroup);

    if (temp.empty()) return false;

    if (!all_of(temp.begin(), temp.end(), [&](const shared_ptr<StreetTile>& streetTile) {
        return streetTile->getOwner() == this->getOwner();
    })) return false;

    return true;
}

void StreetTile::activateFestival() {
    if (festivalMultiplier < 2) festivalMultiplier = 2;
    else if (festivalMultiplier < 8) festivalMultiplier *= 2;
    festivalDuration = 3;
}

void StreetTile::decrementFestivalDuration() {
    if (festivalDuration >= 0) festivalDuration--;
}

void StreetTile::acquire(Game& game, Player& player) {
    if (this->getOwner() != nullptr) throw InvalidActionException("Property already has an owner");
    if (this->ownershipStatus != OwnershipStatus::BANK) throw InvalidActionException("Property is not in BANK status");
    if (player.getMoney() < this->purchasePrice) throw InsufficientFundsException(this->purchasePrice, player.getMoney());
    player.deductMoney(this->purchasePrice);
    this->setOwner(&player);
    this->ownershipStatus = OwnershipStatus::OWNED;
    player.addProperty(this);
    // TODO : might add log, check later

    
}

string StreetTile::getColorGroup() const {
    return this->colorGroup;
}

int StreetTile::getHouseBuildCost() const {
    return houseBuildCost;
}

int StreetTile::getHotelBuildCost() const {
    return hotelBuildCost;
}

const vector<int>& StreetTile::getRentLevels() const {
    return rentLevels;
}

int StreetTile::getFestivalMultiplier() const {
    return festivalMultiplier;
}

int StreetTile::getFestivalDuration() const {
    return festivalDuration;
}

int StreetTile::getBuildingValue() const  {
    if (buildingLevel >= 1 && buildingLevel <= 4) return buildingLevel * houseBuildCost;
    if (buildingLevel == 5) return hotelBuildCost;
    return 0;
}

int StreetTile::getSelltoBankValue() const  {
    return purchasePrice + getBuildingValue() / 2;
}