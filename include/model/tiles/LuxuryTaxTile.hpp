#pragma once

#include "model/tiles/TaxTile.hpp"

using namespace std;

class LuxuryTaxTile : public TaxTile {
public:
    LuxuryTaxTile();
    LuxuryTaxTile(int index, const string& code, const string& name, int taxAmount);
    LuxuryTaxTile(const LuxuryTaxTile& other);
    ~LuxuryTaxTile();
    LuxuryTaxTile& operator=(const LuxuryTaxTile& other);

    int calculateLuxuryTax() const;

    TileType onLand() const override;
};