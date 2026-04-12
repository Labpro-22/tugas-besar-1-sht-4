#pragma once

#include <string>

using namespace std;

class Game;
class Player;

class Tile {
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

    virtual void onLand(Game& game, Player& player) = 0;

    int getIndex() const;
    string getCode() const;
    string getName() const;
};