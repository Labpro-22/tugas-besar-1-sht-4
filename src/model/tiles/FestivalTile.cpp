#include "model/tiles/FestivalTile.hpp"

using namespace std;

FestivalTile::FestivalTile() {}

FestivalTile::FestivalTile(int index, const string& code, const string& name)
    : ActionTile(index, code, name)
{}

FestivalTile::FestivalTile(const FestivalTile& other)
    : ActionTile(other)
{}

FestivalTile::~FestivalTile() {}

FestivalTile& FestivalTile::operator=(const FestivalTile& other) {
    if (this != &other) {
        ActionTile::operator=(other);
    }
    return *this;
}

void FestivalTile::onLand(Game& game, Player& player) {
    // TODO : implement ui or something
}