#include "model/tiles/FestivalTile.hpp"

using namespace std;

FestivalTile::FestivalTile() {}

FestivalTile::FestivalTile(int index, const string& code, const string& name) {}

FestivalTile::FestivalTile(const FestivalTile& other) {}

FestivalTile::~FestivalTile() {}

FestivalTile& FestivalTile::operator=(const FestivalTile& other) {
    return *this;
}

void FestivalTile::onLand(Game& game, Player& player) {}