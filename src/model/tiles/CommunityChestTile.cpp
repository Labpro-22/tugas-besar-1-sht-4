#include "model/tiles/CommunityChestTile.hpp"

using namespace std;

CommunityChestTile::CommunityChestTile() {}

CommunityChestTile::CommunityChestTile(int index, const string& code, const string& name, const string& deckType) {}

CommunityChestTile::CommunityChestTile(const CommunityChestTile& other) {}

CommunityChestTile::~CommunityChestTile() {}

CommunityChestTile& CommunityChestTile::operator=(const CommunityChestTile& other) {
    return *this;
}

void CommunityChestTile::onLand(Game& game, Player& player) {}