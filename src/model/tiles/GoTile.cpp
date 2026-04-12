#include "model/tiles/GoTile.hpp"

using namespace std;

GoTile::GoTile() {}

GoTile::GoTile(int index, const string& code, const string& name, int salary) {}

GoTile::GoTile(const GoTile& other) {}

GoTile::~GoTile() {}

GoTile& GoTile::operator=(const GoTile& other) {
    return *this;
}

void GoTile::onLand(Game& game, Player& player) {}