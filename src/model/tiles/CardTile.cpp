#include "model/tiles/CardTile.hpp"

using namespace std;

CardTile::CardTile() {}

CardTile::CardTile(int index, const string& code, const string& name, const string& deckType) {}

CardTile::CardTile(const CardTile& other) {}

CardTile::~CardTile() {}

CardTile& CardTile::operator=(const CardTile& other) {
    return *this;
}

void CardTile::onLand(Game& game, Player& player) {}