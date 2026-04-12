#pragma once

#include "model/tiles/CardTile.hpp"

using namespace std;

class Game;
class Player;

class ChanceTile : public CardTile {
public:
    ChanceTile();
    ChanceTile(int index, const string& code, const string& name, const string& deckType);
    ChanceTile(const ChanceTile& other);
    ~ChanceTile();
    ChanceTile& operator=(const ChanceTile& other);

    void onLand(Game& game, Player& player) override;
};