#pragma once

#include "model/tiles/TaxTile.hpp"

using namespace std;

class Game;
class Player;

class IncomeTaxTile : public TaxTile {
private:
    int flatTax;
    int percentageTax;

public:
    IncomeTaxTile();
    IncomeTaxTile(
        int index,
        const string& code,
        const string& name,
        int taxAmount,
        int flatTax,
        int percentageTax
    );
    IncomeTaxTile(const IncomeTaxTile& other);
    ~IncomeTaxTile();
    IncomeTaxTile& operator=(const IncomeTaxTile& other);

    int calculateTax(const Game& game, const Player& player) const override;
    void onLand(Game& game, Player& player) override;
};