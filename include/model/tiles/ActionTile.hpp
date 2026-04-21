#pragma once

#include "model/tiles/Tile.hpp"

using namespace std;

class ActionTile : public Tile {
public:
    ActionTile();
    ActionTile(int index, const string& code, const string& name);
    ActionTile(const ActionTile& other);
    virtual ~ActionTile();
    ActionTile& operator=(const ActionTile& other);

    virtual TileType onLand() const override = 0;
};