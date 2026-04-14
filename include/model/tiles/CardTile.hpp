#pragma once

#include <string>

#include "model/tiles/ActionTile.hpp"

using namespace std;

class Game;
class Player;

class CardTile : public ActionTile {
protected:
    string deckType;

public:
    CardTile();
    CardTile(int index, const string& code, const string& name, const string& deckType);
    CardTile(const CardTile& other);
    ~CardTile();
    CardTile& operator=(const CardTile& other);

    void onLand(Game& game, Player& player) override;

    const string& getDeckType() const;
};