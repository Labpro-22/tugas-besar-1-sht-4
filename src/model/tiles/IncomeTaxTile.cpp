#include "model/tiles/IncomeTaxTile.hpp"

using namespace std;

IncomeTaxTile::IncomeTaxTile() {}

IncomeTaxTile::IncomeTaxTile(
    int index,
    const string& code,
    const string& name,
    int taxAmount,
    int flatTax,
    int percentageTax
) {}

IncomeTaxTile::IncomeTaxTile(const IncomeTaxTile& other) {}

IncomeTaxTile::~IncomeTaxTile() {}

IncomeTaxTile& IncomeTaxTile::operator=(const IncomeTaxTile& other) {
    return *this;
}

int IncomeTaxTile::calculateTax(const Game& game, const Player& player) const {
    return 0;
}

void IncomeTaxTile::onLand(Game& game, Player& player) {}