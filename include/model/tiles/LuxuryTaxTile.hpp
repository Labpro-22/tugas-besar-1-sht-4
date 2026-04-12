#pragma once

#include "model/tiles/TaxTile.hpp"

using namespace std;

class Game;
class Player;

class LuxuryTaxTile : public TaxTile {
public:
    LuxuryTaxTile();
    LuxuryTaxTile(int index, const string& code, const string& name, int taxAmount);
    LuxuryTaxTile(const LuxuryTaxTile& other);
    ~LuxuryTaxTile();
    LuxuryTaxTile& operator=(const LuxuryTaxTile& other);

    int calculateTax(const Game& game, const Player& player) const override;
    void onLand(Game& game, Player& player) override;
};