#include "model/tiles/SpecialTile.hpp"

using namespace std;

SpecialTile::SpecialTile()
    : ActionTile()
{}

SpecialTile::SpecialTile(int index, const string& code, const string& name)
    : ActionTile(index, code, name)
{}

SpecialTile::SpecialTile(const SpecialTile& other)
    : ActionTile(other)
{}

SpecialTile::~SpecialTile() {}

SpecialTile& SpecialTile::operator=(const SpecialTile& other) {
    if (this != &other) {
        ActionTile::operator=(other);
    }
    return *this;
}