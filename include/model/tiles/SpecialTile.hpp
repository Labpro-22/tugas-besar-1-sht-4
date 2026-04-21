#pragma once

#include "model/tiles/ActionTile.hpp"

using namespace std;

class SpecialTile : public ActionTile {
public:
    SpecialTile();
    SpecialTile(int index, const string& code, const string& name);
    SpecialTile(const SpecialTile& other);
    virtual ~SpecialTile();
    SpecialTile& operator=(const SpecialTile& other);

    virtual TileType onLand() const override = 0;
};