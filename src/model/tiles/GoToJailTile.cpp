#include "model/tiles/GoToJailTile.hpp"

using namespace std;

GoToJailTile::GoToJailTile() {}

GoToJailTile::GoToJailTile(int index, const string& code, const string& name) {}

GoToJailTile::GoToJailTile(const GoToJailTile& other) {}

GoToJailTile::~GoToJailTile() {}

GoToJailTile& GoToJailTile::operator=(const GoToJailTile& other) {
    return *this;
}

void GoToJailTile::onLand(Game& game, Player& player) {}