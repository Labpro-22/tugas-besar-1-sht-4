#include "model/tiles/ActionTile.hpp"

using namespace std;

ActionTile::ActionTile()
    : Tile()
{}

ActionTile::ActionTile(int index, const string& code, const string& name)
    : Tile(index, code, name)
{}

ActionTile::ActionTile(const ActionTile& other)
    : Tile(other)
{}

ActionTile::~ActionTile() {}

ActionTile& ActionTile::operator=(const ActionTile& other) {
    if (this != &other) {
        Tile::operator=(other);
    }
    return *this;
}