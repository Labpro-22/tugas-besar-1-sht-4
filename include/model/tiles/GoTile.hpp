#pragma once

#include "model/tiles/SpecialTile.hpp"

using namespace std;

class GoTile : public SpecialTile {
private:
    int salary;

public:
    GoTile();
    GoTile(int index, const string& code, const string& name, int salary);
    GoTile(const GoTile& other);
    ~GoTile();
    GoTile& operator=(const GoTile& other);

    TileType onLand() const override;

    int getSalary() const;
};