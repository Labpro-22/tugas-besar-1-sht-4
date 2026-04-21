#pragma once

#include "model/tiles/TaxTile.hpp"

using namespace std;

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

    TileType onLand() const override;

    int getFlatTax() const;
    int getPercentageTax() const;
    int calculateFlatTax() const;
    int calculatePercentageTax(int playerWealth) const;
};
