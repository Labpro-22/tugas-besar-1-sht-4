#include "model/tiles/ChanceTile.hpp"

using namespace std;

ChanceTile::ChanceTile() {}

ChanceTile::ChanceTile(int index, const string& code, const string& name, const string& deckType) {}

ChanceTile::ChanceTile(const ChanceTile& other) {}

ChanceTile::~ChanceTile() {}

ChanceTile& ChanceTile::operator=(const ChanceTile& other) {
    return *this;
}

void ChanceTile::onLand(Game& game, Player& player) {}