#include "model/tiles/LuxuryTaxTile.hpp"

using namespace std;

LuxuryTaxTile::LuxuryTaxTile() {}

LuxuryTaxTile::LuxuryTaxTile(int index, const string& code, const string& name, int taxAmount)
    : TaxTile(index, code, name, taxAmount)
{}

LuxuryTaxTile::LuxuryTaxTile(const LuxuryTaxTile& other)
    : TaxTile(other)
{}

LuxuryTaxTile::~LuxuryTaxTile() {}

LuxuryTaxTile& LuxuryTaxTile::operator=(const LuxuryTaxTile& other) {
    if (this != &other) {
        TaxTile::operator=(other);
    }
    return *this;
}

void LuxuryTaxTile::onLand(Game& game, Player& player) {
    // TODO : do the ui and stuff, perhaps later with controller
}

int LuxuryTaxTile::calculateLuxuryTax() const {
    return taxAmount;
}