#pragma once

#include "model/tiles/ActionTile.hpp"

using namespace std;

class TaxTile : public ActionTile {
protected:
    int taxAmount;

public:
    TaxTile();
    TaxTile(int index, const string& code, const string& name, int taxAmount);
    TaxTile(const TaxTile& other);
    virtual ~TaxTile();
    TaxTile& operator=(const TaxTile& other);

    virtual TileType onLand() const override = 0;

    int getTaxAmount() const;
};