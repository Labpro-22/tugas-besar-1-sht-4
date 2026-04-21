#pragma once

#include "model/tiles/SpecialTile.hpp"

using namespace std;

class FreeParkingTile : public SpecialTile {
public:
    FreeParkingTile();
    FreeParkingTile(int index, const string& code, const string& name);
    FreeParkingTile(const FreeParkingTile& other);
    ~FreeParkingTile();
    FreeParkingTile& operator=(const FreeParkingTile& other);

    TileType onLand() const override;
};