#include "model/managers/PropertyManager.hpp"

using namespace std;

PropertyManager::PropertyManager() {}

PropertyManager::PropertyManager(const PropertyManager& other) {}

PropertyManager::~PropertyManager() {}

PropertyManager& PropertyManager::operator=(const PropertyManager& other) {
    return *this;
}

void PropertyManager::payRent(Game& game, Player& player, OwnableTile& tile) {}

void PropertyManager::mortgageProperty(Game& game, Player& player, OwnableTile& tile) {}

void PropertyManager::redeemProperty(Game& game, Player& player, OwnableTile& tile) {}

void PropertyManager::buildOnStreet(Game& game, Player& player, StreetTile& tile) {}

void PropertyManager::sellBuildingsInColorGroup(Game& game, Player& player, const string& colorGroup) {}

bool PropertyManager::canMortgage(const Game& game, const Player& player, const OwnableTile& tile) const {
    return false;
}

bool PropertyManager::canBuild(const Game& game, const Player& player, const StreetTile& tile) const {
    return false;
}

void PropertyManager::sellPropertyToBank(Game& game, Player& player, OwnableTile& tile) {}

int PropertyManager::calculateSellToBankValue(const OwnableTile& tile) const {
    return 0;
}

vector<shared_ptr<OwnableTile>> PropertyManager::getMortgageableProperties(const Game& game, const Player& player) const {
    return {};
}

vector<shared_ptr<OwnableTile>> PropertyManager::getRedeemableProperties(const Game& game, const Player& player) const {
    return {};
}

vector<string> PropertyManager::getBuildableColorGroups(const Game& game, const Player& player) const {
    return {};
}

vector<shared_ptr<StreetTile>> PropertyManager::getBuildableStreets(const Game& game, const Player& player, const string& colorGroup) const {
    return {};
}