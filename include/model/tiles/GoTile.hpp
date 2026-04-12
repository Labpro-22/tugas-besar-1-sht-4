#pragma once

#include "model/tiles/SpecialTile.hpp"

using namespace std;

class Game;
class Player;

class GoTile : public SpecialTile {
private:
    int salary;

public:
    GoTile();
    GoTile(int index, const string& code, const string& name, int salary);
    GoTile(const GoTile& other);
    ~GoTile();
    GoTile& operator=(const GoTile& other);

    void onLand(Game& game, Player& player) override;
};