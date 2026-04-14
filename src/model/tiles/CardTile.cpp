#include "model/tiles/CardTile.hpp"

using namespace std;

CardTile::CardTile() {}

CardTile::CardTile(int index, const string& code, const string& name, const string& deckType)
    : ActionTile(index, code, name), deckType(deckType)
{}

CardTile::CardTile(const CardTile& other)
    : ActionTile(other), deckType(other.deckType)
{}

CardTile::~CardTile() {}

CardTile& CardTile::operator=(const CardTile& other) {
    if (this != &other) {
        ActionTile::operator=(other);
        this->deckType = other.deckType;
    }
    return *this;
}

void CardTile::onLand(Game& game, Player& player) {
    // TODO : implement ui or something later, check controller
}

const string& CardTile::getDeckType() const {
    return deckType;
}
