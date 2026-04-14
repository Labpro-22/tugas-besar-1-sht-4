#include "model/tiles/CommunityChestTile.hpp"

using namespace std;

CommunityChestTile::CommunityChestTile() {}

CommunityChestTile::CommunityChestTile(int index, const string& code, const string& name, const string& deckType)
    : CardTile(index, code, name, deckType)
{}

CommunityChestTile::CommunityChestTile(const CommunityChestTile& other)
    : CardTile(other)
{}

CommunityChestTile::~CommunityChestTile() {}

CommunityChestTile& CommunityChestTile::operator=(const CommunityChestTile& other) {
    if (this != &other) {
        CardTile::operator=(other);
    }
    return *this;
}

void CommunityChestTile::onLand(Game& game, Player& player) {
    // TODO : implement ui or something
}