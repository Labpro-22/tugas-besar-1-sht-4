#pragma once

#include "model/tiles/TaxTile.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

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

    int calculateLuxuryTax() const;

    void onLand(Game& game, Player& player) override;
};