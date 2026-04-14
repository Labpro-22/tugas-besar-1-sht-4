#pragma once

#include "model/tiles/TaxTile.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

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

    void onLand(Game& game, Player& player) override;

    int getFlatTax() const;
    int getPercentageTax() const;
    int calculateFlatTax() const;
    int calculatePercentageTax(const Game& game, const Player& player) const;
};