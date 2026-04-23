#include "model/Board.hpp"
#include "model/NimonException.hpp"
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

#include <cctype>

using namespace std;

string Board::normalizeConfigKey(string value) const {
    string normalized;
    for (char ch : value) {
        if (isalnum(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(toupper(static_cast<unsigned char>(ch)));
        }
    }
    return normalized;
}

Board::Board()
    : tiles(), tileIndexByCode()
{}

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
        const string propertyType = this->normalizeConfigKey(cfg.getPropertyType());

        if (propertyType == "RAILROAD") {
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

        if (propertyType == "UTILITY") {
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

        if (propertyType != "STREET") {
            throw FileException(
                "Jenis properti tidak dikenal pada ID " +
                to_string(id) + ": " + cfg.getPropertyType()
            );
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

    auto addActionById = [&](int id) {
        const ConfigManager::ActionTileConfig& cfg = configManager.getActionTileConfigById(id);
        const string type = this->normalizeConfigKey(cfg.getTileType());
        const string code = this->normalizeConfigKey(cfg.getCode());

        if (type == "FESTIVAL" || type == "PETAKFESTIVAL") {
            addTile(make_shared<FestivalTile>(id, cfg.getCode(), cfg.getName()));
            return;
        }

        if (type == "KARTU" || type == "CARD" || type == "PETAKKARTU") {
            if (code == "DNU") {
                addTile(make_shared<CommunityChestTile>(
                    id,
                    cfg.getCode(),
                    cfg.getName(),
                    "COMMUNITY_CHEST"
                ));
                return;
            }
            if (code == "KSP") {
                addTile(make_shared<ChanceTile>(id, cfg.getCode(), cfg.getName(), "CHANCE"));
                return;
            }
        }

        if (type == "PAJAK" || type == "TAX" || type == "PETAKPAJAK") {
            if (code == "PPH") {
                addTile(make_shared<IncomeTaxTile>(
                    id,
                    cfg.getCode(),
                    cfg.getName(),
                    configManager.getPphFlat(),
                    configManager.getPphFlat(),
                    configManager.getPphPercent()
                ));
                return;
            }
            if (code == "PBM") {
                addTile(make_shared<LuxuryTaxTile>(
                    id,
                    cfg.getCode(),
                    cfg.getName(),
                    configManager.getPbmFlat()
                ));
                return;
            }
        }

        if (type == "SPESIAL" || type == "SPECIAL" || type == "PETAKSPESIAL") {
            if (code == "GO") {
                addTile(make_shared<GoTile>(
                    id,
                    cfg.getCode(),
                    cfg.getName(),
                    configManager.getGoSalary()
                ));
                return;
            }
            if (code == "PEN") {
                addTile(make_shared<JailTile>(
                    id,
                    cfg.getCode(),
                    cfg.getName(),
                    configManager.getJailFine()
                ));
                return;
            }
            if (code == "BBP") {
                addTile(make_shared<FreeParkingTile>(id, cfg.getCode(), cfg.getName()));
                return;
            }
            if (code == "PPJ") {
                addTile(make_shared<GoToJailTile>(id, cfg.getCode(), cfg.getName()));
                return;
            }
        }

        throw FileException(
            "Konfigurasi petak aksi tidak dikenal pada ID " +
            to_string(id) + ": " + cfg.getCode() + " " + cfg.getTileType()
        );
    };

    for (int id = 1; id <= 40; id++) {
        if (configManager.hasPropertyId(id)) {
            addPropertyById(id);
        } else if (configManager.hasActionTileId(id)) {
            addActionById(id);
        } else {
            throw FileException("Konfigurasi petak tidak ditemukan untuk ID " + to_string(id));
        }
    }
}

const vector<shared_ptr<Tile>>& Board::getTiles() const {
    return tiles;
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
