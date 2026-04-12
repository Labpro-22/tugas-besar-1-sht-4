#pragma once

#include "model/tiles/SpecialTile.hpp"

using namespace std;

class Game;
class Player;

class GoToJailTile : public SpecialTile {
public:
    GoToJailTile();
    GoToJailTile(int index, const string& code, const string& name);
    GoToJailTile(const GoToJailTile& other);
    ~GoToJailTile();
    GoToJailTile& operator=(const GoToJailTile& other);

    void onLand(Game& game, Player& player) override;
};