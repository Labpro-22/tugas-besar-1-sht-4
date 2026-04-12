#include "model/tiles/LuxuryTaxTile.hpp"

using namespace std;

LuxuryTaxTile::LuxuryTaxTile() {}

LuxuryTaxTile::LuxuryTaxTile(int index, const string& code, const string& name, int taxAmount) {}

LuxuryTaxTile::LuxuryTaxTile(const LuxuryTaxTile& other) {}

LuxuryTaxTile::~LuxuryTaxTile() {}

LuxuryTaxTile& LuxuryTaxTile::operator=(const LuxuryTaxTile& other) {
    return *this;
}

int LuxuryTaxTile::calculateTax(const Game& game, const Player& player) const {
    return 0;
}

void LuxuryTaxTile::onLand(Game& game, Player& player) {}