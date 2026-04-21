#include "model/tiles/ChanceTile.hpp"

using namespace std;

ChanceTile::ChanceTile() {}

ChanceTile::ChanceTile(int index, const string& code, const string& name, const string& deckType)
    : CardTile(index, code, name, deckType)
{}

ChanceTile::ChanceTile(const ChanceTile& other)
    : CardTile(other)
{}

ChanceTile::~ChanceTile() {}

ChanceTile& ChanceTile::operator=(const ChanceTile& other) {
    if (this != &other) {
        CardTile::operator=(other);
    }
    return *this;
}

Tile::TileType ChanceTile::onLand() const {
    return TileType::Chance;
}