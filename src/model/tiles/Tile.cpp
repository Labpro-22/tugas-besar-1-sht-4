#include "model/tiles/Tile.hpp"

using namespace std;

Tile::Tile() {}

Tile::Tile(int index, const string& code, const string& name) {}

Tile::Tile(const Tile& other) {}

Tile::~Tile() {}

Tile& Tile::operator=(const Tile& other) {
    return *this;
}

int Tile::getIndex() const {
    return index;
}

string Tile::getCode() const {
    return code;
}

string Tile::getName() const {
    return name;
}