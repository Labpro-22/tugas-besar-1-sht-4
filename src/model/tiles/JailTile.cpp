#include "model/tiles/JailTile.hpp"

using namespace std;

JailTile::JailTile() {}

JailTile::JailTile(int index, const string& code, const string& name, int jailFine) {}

JailTile::JailTile(const JailTile& other) {}

JailTile::~JailTile() {}

JailTile& JailTile::operator=(const JailTile& other) {
    return *this;
}

void JailTile::onLand(Game& game, Player& player) {}