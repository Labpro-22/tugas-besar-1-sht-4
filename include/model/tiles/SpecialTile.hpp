#pragma once

#include "model/tiles/ActionTile.hpp"

using namespace std;

class Game;
class Player;

class SpecialTile : public ActionTile {
public:
    SpecialTile();
    SpecialTile(int index, const string& code, const string& name);
    SpecialTile(const SpecialTile& other);
    virtual ~SpecialTile();
    SpecialTile& operator=(const SpecialTile& other);

    virtual void onLand(Game& game, Player& player) override = 0;
};