#include "model/tiles/TaxTile.hpp"

using namespace std;

TaxTile::TaxTile() {}

TaxTile::TaxTile(int index, const string& code, const string& name, int taxAmount)
    : ActionTile(index, code, name), taxAmount(taxAmount)
{}

TaxTile::TaxTile(const TaxTile& other)
    : ActionTile(other), taxAmount(other.taxAmount)
{}

TaxTile::~TaxTile() {}

TaxTile& TaxTile::operator=(const TaxTile& other) {
    if (this != &other) {
        ActionTile::operator=(other);
        this->taxAmount = other.taxAmount;
    }
    return *this;
}

void TaxTile::onLand(Game& game, Player& player) {
    // TODO : implement ui or something 
}


int TaxTile::getTaxAmount() const {
    return taxAmount;
}