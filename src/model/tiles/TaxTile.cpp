#include "model/tiles/TaxTile.hpp"

using namespace std;

TaxTile::TaxTile() {}

TaxTile::TaxTile(int index, const string& code, const string& name, int taxAmount) {}

TaxTile::TaxTile(const TaxTile& other) {}

TaxTile::~TaxTile() {}

TaxTile& TaxTile::operator=(const TaxTile& other) {
    return *this;
}

void TaxTile::onLand(Game& game, Player& player) {}