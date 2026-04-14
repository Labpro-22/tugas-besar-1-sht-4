#include "model/Board.hpp"
#include "model/tiles/Tile.hpp"

using namespace std;

Board::Board() {}

Board::Board(const vector<shared_ptr<Tile>>& tiles, const map<string, int>& tileIndexByCode) 
    : tiles(tiles), tileIndexByCode(tileIndexByCode)
{}

Board::Board(const Board& other) 
    : tiles(other.tiles), tileIndexByCode(other.tileIndexByCode)
{}

Board::~Board() {}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        this->tiles = other.tiles;
        this->tileIndexByCode = other.tileIndexByCode;
    }
    return *this;
}

void Board::initializeBoard() {
    // TODO : might have to initialize by config, not sure yet check configmanager
}

shared_ptr<Tile> Board::getTile(int index) const {
    auto it = find_if(tiles.begin(), tiles.end(), [&](const shared_ptr<Tile>& tile) {
        return tile->getIndex() == index;
    });
    if (it == tiles.end()) return nullptr;
    return *it;
}

shared_ptr<Tile> Board::getTileByCode(const string& code) const {
    auto it = find_if(tiles.begin(), tiles.end(), [&](const shared_ptr<Tile>& tile) {
        return tile->getCode() == code;
    });
    if (it == tiles.end()) return nullptr;
    return *it;
}

int Board::getTileIndex(const string& code) const {
    auto it = find_if(tiles.begin(), tiles.end(), [&](const shared_ptr<Tile>& tile) {
        return tile->getCode() == code;
    });
    if (it == tiles.end()) return 0;
    return (*it)->getIndex();
}

int Board::getNextPosition(int currentPos, int steps) const {
    return (currentPos + steps) % tiles.size();
}

int Board::getBoardSize() const {
    return tiles.size();
}

vector<shared_ptr<StreetTile>> Board::getStreetTileByColorGroup(const string& colorGroup) const {
    vector<shared_ptr<StreetTile>> temp;
    for (const auto& tile : tiles) {
        shared_ptr<StreetTile> street = dynamic_pointer_cast<StreetTile>(tile);
        if (street != nullptr && street->getColorGroup() == colorGroup) {
            temp.push_back(street);
        }
    }
    return temp;
}

int Board::countRailroadsOwned(const Player& player) const {
    return count_if(tiles.begin(), tiles.end(), [&](const shared_ptr<Tile>& tile) {
        shared_ptr<RailroadTile> railroad = dynamic_pointer_cast<RailroadTile>(tile);
        if (railroad != nullptr) return railroad->getOwner() == &player;
        return false;
    });
}

int Board::countUtilitiesOwned(const Player& player) const {
    return count_if(tiles.begin(), tiles.end(), [&](const shared_ptr<Tile>& tile) {
        shared_ptr<UtilityTile> utility = dynamic_pointer_cast<UtilityTile>(tile);
        if (utility != nullptr) return utility->getOwner() == &player;
        return false;
    });
}