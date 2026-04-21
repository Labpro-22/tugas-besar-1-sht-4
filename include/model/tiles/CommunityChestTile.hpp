#pragma once

#include "model/tiles/CardTile.hpp"

using namespace std;

class CommunityChestTile : public CardTile {
public:
    CommunityChestTile();
    CommunityChestTile(int index, const string& code, const string& name, const string& deckType);
    CommunityChestTile(const CommunityChestTile& other);
    ~CommunityChestTile();
    CommunityChestTile& operator=(const CommunityChestTile& other);

    TileType onLand() const override;
};