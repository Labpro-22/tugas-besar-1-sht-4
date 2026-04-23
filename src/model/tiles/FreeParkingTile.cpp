#include "model/tiles/FreeParkingTile.hpp"

using namespace std;

FreeParkingTile::FreeParkingTile()
    : SpecialTile()
{}

FreeParkingTile::FreeParkingTile(int index, const string& code, const string& name)
    : SpecialTile(index, code, name)
{}

FreeParkingTile::FreeParkingTile(const FreeParkingTile& other)
    : SpecialTile(other)
{}

FreeParkingTile::~FreeParkingTile() {}

FreeParkingTile& FreeParkingTile::operator=(const FreeParkingTile& other) {
    if (this != &other) {
        SpecialTile::operator=(other);
    }
    return *this;
}

Tile::TileType FreeParkingTile::onLand() const {
    return TileType::FreeParking;
}