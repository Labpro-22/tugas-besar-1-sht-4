#include "model/Board.hpp"
#include "model/tiles/Tile.hpp"

using namespace std;

Board::Board() {}

Board::Board(const vector<shared_ptr<Tile>>& tiles, const map<string, int>& tileIndexByCode) {}

Board::Board(const Board& other) {}

Board::~Board() {}

Board& Board::operator=(const Board& other) {
    return *this;
}

void Board::initializeBoard() {}

shared_ptr<Tile> Board::getTile(int index) const {
    return nullptr;
}

shared_ptr<Tile> Board::getTileByCode(const string& code) const {
    return nullptr;
}

int Board::getTileIndex(const string& code) const {
    return -1;
}

int Board::getNextPosition(int currentPos, int steps) const {
    return 0;
}

int Board::size() const {
    return 0;
}