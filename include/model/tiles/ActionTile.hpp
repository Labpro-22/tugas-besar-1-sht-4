#pragma once

#include "model/tiles/Tile.hpp"

using namespace std;

class Game;
class Player;

class ActionTile : public Tile {
public:
    ActionTile();
    ActionTile(int index, const string& code, const string& name);
    ActionTile(const ActionTile& other);
    virtual ~ActionTile();
    ActionTile& operator=(const ActionTile& other);

    virtual void onLand(Game& game, Player& player) override = 0;
};