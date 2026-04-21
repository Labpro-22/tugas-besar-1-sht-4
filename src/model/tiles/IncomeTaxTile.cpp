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
)
    : TaxTile(index, code, name, taxAmount),
    flatTax(flatTax),
    percentageTax(percentageTax)
{}

IncomeTaxTile::IncomeTaxTile(const IncomeTaxTile& other)
    : TaxTile(other),
    flatTax(other.flatTax),
    percentageTax(other.percentageTax)
{}

IncomeTaxTile::~IncomeTaxTile() {}

IncomeTaxTile& IncomeTaxTile::operator=(const IncomeTaxTile& other) {
    if (this != &other) {
        TaxTile::operator=(other);
        this->flatTax = other.flatTax;
        this->percentageTax = other.percentageTax;
    }
    return *this;
}

Tile::TileType IncomeTaxTile::onLand() const {
    return TileType::IncomeTax;
}

int IncomeTaxTile::getFlatTax() const {
    return flatTax;
}

int IncomeTaxTile::getPercentageTax() const {
    return percentageTax;
}

int IncomeTaxTile::calculateFlatTax() const {
    return flatTax;
}

int IncomeTaxTile::calculatePercentageTax(int playerWealth) const {
    // TODO : Make sure that total wealth is money + buildings 
    return playerWealth * percentageTax / 100;
}