#include "model/tiles/LuxuryTaxTile.hpp"

using namespace std;

LuxuryTaxTile::LuxuryTaxTile()
    : TaxTile()
{}

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

Tile::TileType LuxuryTaxTile::onLand() const {
    return TileType::LuxuryTax;
}

int LuxuryTaxTile::calculateLuxuryTax() const {
    return taxAmount;
}