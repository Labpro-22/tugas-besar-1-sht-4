#pragma once

#include "model/tiles/CardTile.hpp"

using namespace std;

class ChanceTile : public CardTile {
public:
    ChanceTile();
    ChanceTile(int index, const string& code, const string& name, const string& deckType);
    ChanceTile(const ChanceTile& other);
    ~ChanceTile();
    ChanceTile& operator=(const ChanceTile& other);

    TileType onLand() const override;
};