#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/UtilityTile.hpp"

using namespace std;

class ConfigManager;
class Player;
class Tile;

class Board {
private:
    vector<shared_ptr<Tile>> tiles;
    map<string, int> tileIndexByCode;

public:
    Board();
    Board(const vector<shared_ptr<Tile>>& tiles, const map<string, int>& tileIndexByCode);
    Board(const Board& other);
    ~Board();
    Board& operator=(const Board& other);

    void initializeBoard(const ConfigManager& configManager);
    shared_ptr<Tile> getTile(int index) const;
    shared_ptr<Tile> getTileByCode(const string& code) const;
    int getTileIndex(const string& code) const;
    int getNextPosition(int currentPos, int steps) const;
    int getBoardSize() const;
    vector<shared_ptr<StreetTile>> getStreetTileByColorGroup(const string& colorGroup) const;
    int countRailroadsOwned(const Player& player) const;
    int countUtilitiesOwned(const Player& player) const;
};
