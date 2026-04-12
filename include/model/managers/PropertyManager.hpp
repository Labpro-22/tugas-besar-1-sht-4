#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class Game;
class Player;
class OwnableTile;
class StreetTile;

class PropertyManager {
public:
    PropertyManager();
    PropertyManager(const PropertyManager& other);
    ~PropertyManager();
    PropertyManager& operator=(const PropertyManager& other);

    void payRent(Game& game, Player& player, OwnableTile& tile);
    void mortgageProperty(Game& game, Player& player, OwnableTile& tile);
    void redeemProperty(Game& game, Player& player, OwnableTile& tile);
    void buildOnStreet(Game& game, Player& player, StreetTile& tile);
    void sellBuildingsInColorGroup(Game& game, Player& player, const string& colorGroup);
    bool canMortgage(const Game& game, const Player& player, const OwnableTile& tile) const;
    bool canBuild(const Game& game, const Player& player, const StreetTile& tile) const;
    void sellPropertyToBank(Game& game, Player& player, OwnableTile& tile);
    int calculateSellToBankValue(const OwnableTile& tile) const;
    vector<shared_ptr<OwnableTile>> getMortgageableProperties(const Game& game, const Player& player) const;
    vector<shared_ptr<OwnableTile>> getRedeemableProperties(const Game& game, const Player& player) const;
    vector<string> getBuildableColorGroups(const Game& game, const Player& player) const;
    vector<shared_ptr<StreetTile>> getBuildableStreets(const Game& game, const Player& player, const string& colorGroup) const;
};