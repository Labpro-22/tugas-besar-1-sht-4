#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Tile;

class Board {
private:
    vector<shared_ptr<Tile>> tiles;
    map<string, int> tileIndexByCode;

public:
    Board();
    Board(const vector<shared_ptr<Tile>>& tiles, const map<string, int>& tileIndexByCode);
    Board(const Board& other);
    ~Board();
    Board& operator=(const Board& other);

    void initializeBoard();
    shared_ptr<Tile> getTile(int index) const;
    shared_ptr<Tile> getTileByCode(const string& code) const;
    int getTileIndex(const string& code) const;
    int getNextPosition(int currentPos, int steps) const;
    int size() const;
};