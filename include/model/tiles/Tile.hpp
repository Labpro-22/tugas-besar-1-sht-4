#pragma once

#include <string>

using namespace std;

class Tile {
public:
    enum class TileType {
        Festival = 0,
        Street,
        Railroad,
        Utility,
        IncomeTax,
        LuxuryTax,
        Chance,
        CommunityChest,
        Go,
        Jail,
        GoToJail,
        FreeParking
    };

protected:
    int index;
    string code;
    string name;

public:
    Tile();
    Tile(int index, const string& code, const string& name);
    Tile(const Tile& other);
    virtual ~Tile();
    Tile& operator=(const Tile& other);

    virtual TileType onLand() const = 0;

    int getIndex() const;
    string getCode() const;
    string getName() const;
};