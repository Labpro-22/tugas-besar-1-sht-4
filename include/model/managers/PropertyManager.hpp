#pragma once

#include <memory>
#include <string>
#include <vector>

#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/GameContext.hpp"
using namespace std;


class PropertyManager {
public:
    PropertyManager();
    PropertyManager(const PropertyManager& other);
    ~PropertyManager();
    PropertyManager& operator=(const PropertyManager& other);

    void payRent(Player& player, OwnableTile& tile);
    void mortgageProperty(Player& player, OwnableTile& tile);
    void redeemProperty(Player& player, OwnableTile& tile);
    void buildOnStreet(Player& player, StreetTile& tile);
    void sellBuildingsInColorGroup(Player& player, const string& colorGroup);
    bool canMortgage(const Player& player, const OwnableTile& tile) const;
    bool canBuild(const Player& player, const StreetTile& tile) const;
    void sellPropertyToBank(Player& player, OwnableTile& tile);
    int calculateSellToBankValue(const OwnableTile& tile) const;
    vector<shared_ptr<OwnableTile>> getMortgageableProperties(const Game& game, const Player& player) const;
    vector<shared_ptr<OwnableTile>> getRedeemableProperties(const Game& game, const Player& player) const;
    vector<string> getBuildableColorGroups(const Game& game, const Player& player) const;
    vector<shared_ptr<StreetTile>> getBuildableStreets(const Game& game, const Player& player, const string& colorGroup) const;
};