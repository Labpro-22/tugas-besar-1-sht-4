#include "model/tiles/GoToJailTile.hpp"

using namespace std;

GoToJailTile::GoToJailTile() {}

GoToJailTile::GoToJailTile(int index, const string& code, const string& name)
    : SpecialTile(index, code, name)
{}

GoToJailTile::GoToJailTile(const GoToJailTile& other)
    : SpecialTile(other)
{}

GoToJailTile::~GoToJailTile() {}

GoToJailTile& GoToJailTile::operator=(const GoToJailTile& other) {
    if (this != &other) {
        SpecialTile::operator=(other);
    }
    return *this;
}
void GoToJailTile::onLand(Game& game, Player& player) {
    // TODO : do this later 
}