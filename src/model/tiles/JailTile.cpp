#include "model/tiles/JailTile.hpp"

using namespace std;

JailTile::JailTile() {}

JailTile::JailTile(int index, const string& code, const string& name, int jailFine)
    : SpecialTile(index, code, name), jailFine(jailFine)
{}

JailTile::JailTile(const JailTile& other)
    : SpecialTile(other), jailFine(other.jailFine)
{}

JailTile::~JailTile() {}

JailTile& JailTile::operator=(const JailTile& other) {
    if (this != &other) {
        SpecialTile::operator=(other);
        this->jailFine = other.jailFine;
    }
    return *this;
}

Tile::TileType JailTile::onLand() const {
    return TileType::Jail;
}


int JailTile::getJailFine() const {
    return jailFine;
}