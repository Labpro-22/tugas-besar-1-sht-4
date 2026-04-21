#pragma once

#include "model/tiles/SpecialTile.hpp"

using namespace std;

class GoToJailTile : public SpecialTile {
public:
    GoToJailTile();
    GoToJailTile(int index, const string& code, const string& name);
    GoToJailTile(const GoToJailTile& other);
    ~GoToJailTile();
    GoToJailTile& operator=(const GoToJailTile& other);

    TileType onLand() const override;
};