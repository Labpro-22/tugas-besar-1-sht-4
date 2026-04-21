#pragma once

#include "model/tiles/SpecialTile.hpp"

using namespace std;

class JailTile : public SpecialTile {
private:
    int jailFine;

public:
    JailTile();
    JailTile(int index, const string& code, const string& name, int jailFine);
    JailTile(const JailTile& other);
    ~JailTile();
    JailTile& operator=(const JailTile& other);

    TileType onLand() const override;

    int getJailFine() const;
};