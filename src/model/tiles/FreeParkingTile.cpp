#include "model/tiles/FreeParkingTile.hpp"

using namespace std;

FreeParkingTile::FreeParkingTile() {}

FreeParkingTile::FreeParkingTile(int index, const string& code, const string& name) {}

FreeParkingTile::FreeParkingTile(const FreeParkingTile& other) {}

FreeParkingTile::~FreeParkingTile() {}

FreeParkingTile& FreeParkingTile::operator=(const FreeParkingTile& other) {
    return *this;
}

void FreeParkingTile::onLand(Game& game, Player& player) {}