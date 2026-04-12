#pragma once

#include "model/tiles/ActionTile.hpp"

using namespace std;

class Game;
class Player;

class FestivalTile : public ActionTile {
public:
    FestivalTile();
    FestivalTile(int index, const string& code, const string& name);
    FestivalTile(const FestivalTile& other);
    ~FestivalTile();
    FestivalTile& operator=(const FestivalTile& other);

    void onLand(Game& game, Player& player) override;
};