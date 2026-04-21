#include "model/Board.hpp"
#include "model/managers/ConfigManager.hpp"

#include "model/tiles/Tile.hpp"
#include "model/tiles/GoTile.hpp"
#include "model/tiles/JailTile.hpp"
#include "model/tiles/FreeParkingTile.hpp"
#include "model/tiles/GoToJailTile.hpp"
#include "model/tiles/IncomeTaxTile.hpp"
#include "model/tiles/LuxuryTaxTile.hpp"
#include "model/tiles/FestivalTile.hpp"
#include "model/tiles/ChanceTile.hpp"
#include "model/tiles/CommunityChestTile.hpp"

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

void Board::initializeBoard(const ConfigManager& configManager) {
    tiles.clear();
    tileIndexByCode.clear();

    auto addTile = [&](const shared_ptr<Tile>& tile) {
        tiles.push_back(tile);

        if (tileIndexByCode.find(tile->getCode()) == tileIndexByCode.end()) {
            tileIndexByCode[tile->getCode()] = tile->getIndex();
        }
    };

    auto addPropertyById = [&](int id) {
        const ConfigManager::PropertyConfig& cfg = configManager.getPropertyConfigById(id);

        if (id == 6 || id == 16 || id == 26 || id == 36) {
            addTile(make_shared<RailroadTile>(
                id,
                cfg.getCode(),
                cfg.getName(),
                nullptr,
                OwnershipStatus::BANK,
                cfg.getPurchasePrice(),
                cfg.getMortgageValue()
            ));
            return;
        }

        if (id == 13 || id == 29) {
            addTile(make_shared<UtilityTile>(
                id,
                cfg.getCode(),
                cfg.getName(),
                nullptr,
                OwnershipStatus::BANK,
                cfg.getPurchasePrice(),
                cfg.getMortgageValue()
            ));
            return;
        }

        addTile(make_shared<StreetTile>(
            id,
            cfg.getCode(),
            cfg.getName(),
            nullptr,
            OwnershipStatus::BANK,
            cfg.getPurchasePrice(),
            cfg.getMortgageValue(),
            cfg.getColorGroup(),
            cfg.getHouseBuildCost(),
            cfg.getHotelBuildCost(),
            cfg.getRentLevels(),
            0,
            1,
            0
        ));
    };

    addTile(make_shared<GoTile>(1, "GO", "Petak Mulai", configManager.getGoSalary()));
    addPropertyById(2);
    addTile(make_shared<CommunityChestTile>(3, "DNU", "Dana Umum", "COMMUNITY_CHEST"));
    addPropertyById(4);
    addTile(make_shared<IncomeTaxTile>(
        5,
        "PPH",
        "Pajak Penghasilan",
        configManager.getPphFlat(),
        configManager.getPphFlat(),
        configManager.getPphPercent()
    ));
    addPropertyById(6);
    addPropertyById(7);
    addTile(make_shared<FestivalTile>(8, "FES", "Festival"));
    addPropertyById(9);
    addPropertyById(10);
    addTile(make_shared<JailTile>(11, "PEN", "Penjara", configManager.getJailFine()));
    addPropertyById(12);
    addPropertyById(13);
    addPropertyById(14);
    addPropertyById(15);
    addPropertyById(16);
    addPropertyById(17);
    addTile(make_shared<CommunityChestTile>(18, "DNU", "Dana Umum", "COMMUNITY_CHEST"));
    addPropertyById(19);
    addPropertyById(20);
    addTile(make_shared<FreeParkingTile>(21, "BBP", "Bebas Parkir"));
    addPropertyById(22);
    addTile(make_shared<ChanceTile>(23, "KSP", "Kesempatan", "CHANCE"));
    addPropertyById(24);
    addPropertyById(25);
    addPropertyById(26);
    addPropertyById(27);
    addPropertyById(28);
    addPropertyById(29);
    addPropertyById(30);
    addTile(make_shared<GoToJailTile>(31, "PPJ", "Petak Pergi ke Penjara"));
    addPropertyById(32);
    addPropertyById(33);
    addTile(make_shared<FestivalTile>(34, "FES", "Festival"));
    addPropertyById(35);
    addPropertyById(36);
    addTile(make_shared<ChanceTile>(37, "KSP", "Kesempatan", "CHANCE"));
    addPropertyById(38);
    addTile(make_shared<LuxuryTaxTile>(
        39,
        "PBM",
        "Pajak Barang Mewah",
        configManager.getPbmFlat()
    ));
    addPropertyById(40);
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