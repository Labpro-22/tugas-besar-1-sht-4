#include "model/tiles/FestivalTile.hpp"

using namespace std;

FestivalTile::FestivalTile() {}

FestivalTile::FestivalTile(int index, const string& code, const string& name)
    : ActionTile(index, code, name)
{}

FestivalTile::FestivalTile(const FestivalTile& other)
    : ActionTile(other)
{}

FestivalTile::~FestivalTile() {}

FestivalTile& FestivalTile::operator=(const FestivalTile& other) {
    if (this != &other) {
        ActionTile::operator=(other);
    }
    return *this;
}

Tile::TileType FestivalTile::onLand() const {
    return TileType::Festival;
}