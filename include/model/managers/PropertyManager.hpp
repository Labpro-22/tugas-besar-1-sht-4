#pragma once

#include <memory>
#include <string>
#include <vector>

#include "model/RentContext.hpp"

using namespace std;

class Board;
class ConfigManager;
class Dice;
class Game;
class OwnableTile;
class Player;
class StreetTile;

class PropertyManager {
public:
    PropertyManager();
    PropertyManager(const PropertyManager& other);
    ~PropertyManager();
    PropertyManager& operator=(const PropertyManager& other);

    RentContext createRentContext(const Board& board, const ConfigManager& configManager, const Dice& dice, const OwnableTile& tile) const;

    void payRent(const Board& board, const ConfigManager& configManager, const Dice& dice, Player& player, OwnableTile& tile);
    void mortgageProperty(Player& player, OwnableTile& tile);
    void redeemProperty(Player& player, OwnableTile& tile);
    void buildOnStreet(const Board& board, Player& player, StreetTile& tile);
    void sellBuildingsInColorGroup(Player& player, const string& colorGroup);
    bool canMortgage(const Player& player, const OwnableTile& tile) const;
    bool canBuild(const Board& board, const Player& player, const StreetTile& tile) const;
    bool canBuildHouse(const Board& board, const Player& player, const StreetTile& tile) const;
    bool canBuildHotel(const Board& board, const Player& player, const StreetTile& tile) const;
    bool isMonopoly(const Board& board, const StreetTile& tile) const;
    void sellPropertyToBank(Player& player, OwnableTile& tile);
    int calculateSellToBankValue(const OwnableTile& tile) const;
    vector<OwnableTile*> getOwnedProperties(const Board& board, const Player& player) const;
    int countOwnedProperties(const Board& board, const Player& player) const;
    int calculateTotalWealth(const Board& board, const Player& player) const;
    vector<OwnableTile*> getMortgageableProperties(const Game& game, const Player& player) const;
    vector<OwnableTile*> getRedeemableProperties(const Game& game, const Player& player) const;
    vector<string> getBuildableColorGroups(const Game& game, const Player& player) const;
    vector<StreetTile*> getBuildableStreets(const Game& game, const Player& player, const string& colorGroup) const;
};