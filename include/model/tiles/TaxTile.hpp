#pragma once

#include "model/tiles/ActionTile.hpp"

using namespace std;

class Game;
class Player;

class TaxTile : public ActionTile {
protected:
    int taxAmount;

public:
    TaxTile();
    TaxTile(int index, const string& code, const string& name, int taxAmount);
    TaxTile(const TaxTile& other);
    virtual ~TaxTile();
    TaxTile& operator=(const TaxTile& other);

    virtual int calculateTax(const Game& game, const Player& player) const = 0;
    void onLand(Game& game, Player& player) override;
};