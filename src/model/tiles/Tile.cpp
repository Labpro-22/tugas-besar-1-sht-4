#include "model/tiles/Tile.hpp"

using namespace std;

Tile::Tile()
    : index(0), code(""), name("")
{}

Tile::Tile(int index, const string& code, const string& name) : index(index), code(code), name(name) {}

Tile::Tile(const Tile& other) : index(other.index), code(other.code), name(other.name)  {}

// TODO : mungkin implement destructor
Tile::~Tile() {}

Tile& Tile::operator=(const Tile& other) {
    if (this != &other) {
        this->code = other.code;
        this->index = other.index;
        this->name = other.name;
    }
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