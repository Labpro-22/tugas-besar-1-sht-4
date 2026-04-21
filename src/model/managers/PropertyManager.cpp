#include "model/managers/PropertyManager.hpp"

using namespace std;

PropertyManager::PropertyManager() {}

PropertyManager::PropertyManager(const PropertyManager& other) {}

PropertyManager::~PropertyManager() {}

PropertyManager& PropertyManager::operator=(const PropertyManager& other) {
    return *this;
}


void PropertyManager::payRent(Player& player, OwnableTile& tile) {
    Player* owner = tile.getOwner();
    int rentAmount = tile.calculateRent(game, player);
    if (rentAmount > player.getMoney()) {
        throw InsufficientFundsException(rentAmount, player.getMoney());
    }
    player -= rentAmount;
    *owner += rentAmount;
}

void PropertyManager::mortgageProperty(Player& player, OwnableTile& tile) {
    if (!canMortgage(game, player, tile)) return;
    tile.mortgage();
    player += tile.getMortgageValue();
}

void PropertyManager::redeemProperty(Player& player, OwnableTile& tile) {
    if (!tile.isMortgaged()) return;
    int redeemCost = tile.getMortgageValue();
    player -= redeemCost;
    tile.redeem();
}

void PropertyManager::buildOnStreet(Player& player, StreetTile& tile) {
    if (tile.canBuildHouse(game)) {
        tile.buildHouse();
        player -= tile.getHouseBuildCost();
    } else if (tile.canBuildHotel(game)) {
        tile.buildHotel();
        player -= tile.getHotelBuildCost();
    }
}

void PropertyManager::sellBuildingsInColorGroup(Player& player, const string& colorGroup) {}

bool PropertyManager::canMortgage(const Player& player, const OwnableTile& tile) const {
    if (tile.isOwned() && tile.getOwner() == &player && !tile.isMortgaged()) return true;
    return false;
}

bool PropertyManager::canBuild(const Player& player, const StreetTile& tile) const {
    if (tile.isOwned() && tile.getOwner() == &player && tile.isMonopoly(game) && !tile.hasHotel()) return true;
    return false;
}

void PropertyManager::sellPropertyToBank(Player& player, OwnableTile& tile) {
    if (!tile.isOwned() || tile.getOwner() != &player) return;
    int sellValue = calculateSellToBankValue(tile);
    player += sellValue;
    tile.setOwner(nullptr);
}

int PropertyManager::calculateSellToBankValue(const OwnableTile& tile) const {
    //TODO : Check spek buat tau giman caluclate Sell
    return 0;
}

vector<shared_ptr<OwnableTile>> PropertyManager::getMortgageableProperties(const Player& player) const {
    vector<shared_ptr<OwnableTile>> mortgageableProperties;
    for (const auto& property : player.getOwnedProperties()) {
        if (canMortgage(game, player, *property)) {
            mortgageableProperties.push_back(make_shared<OwnableTile>(*property));
        }
    }
    return mortgageableProperties;
}

vector<shared_ptr<OwnableTile>> PropertyManager::getRedeemableProperties(const Player& player) const {
    vector<shared_ptr<OwnableTile>> redeemableProperties;
    for (const auto& property : player.getOwnedProperties()) {
        if (property->isMortgaged()) {
            redeemableProperties.push_back(make_shared<OwnableTile>(*property));
        }
    }
    return redeemableProperties;
}

vector<string> PropertyManager::getBuildableColorGroups(const Player& player) const {
    vector<string> buildableColorGroups;
    for (const auto& property : player.getOwnedProperties()) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr && canBuild(game, player, *street)) {
            if (find(buildableColorGroups.begin(), buildableColorGroups.end(), street->getColorGroup()) == buildableColorGroups.end()) {
                buildableColorGroups.push_back(street->getColorGroup());
            }
        }
    }
    return buildableColorGroups;
}

vector<shared_ptr<StreetTile>> PropertyManager::getBuildableStreets(const Player& player, const string& colorGroup) const {
    vector<shared_ptr<StreetTile>> buildableStreets;
    for (const auto& property : player.getOwnedProperties()) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr && canBuild(game, player, *street) && street->getColorGroup() == colorGroup) {
            buildableStreets.push_back(make_shared<StreetTile>(*street));
        }
    }
    return buildableStreets;
}