#pragma once

#include <string>

#include "model/tiles/ActionTile.hpp"

using namespace std;

class CardTile : public ActionTile {
protected:
    string deckType;

public:
    CardTile();
    CardTile(int index, const string& code, const string& name, const string& deckType);
    CardTile(const CardTile& other);
    ~CardTile();
    CardTile& operator=(const CardTile& other);

    virtual TileType onLand() const override = 0;

    const string& getDeckType() const;
};