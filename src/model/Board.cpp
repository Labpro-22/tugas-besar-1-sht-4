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

    auto addPropertyByCode = [&](const string& code, int tileIndex) {
        const ConfigManager::PropertyConfig& cfg = configManager.getPropertyConfig(code);
        const string propertyType = this->normalizeConfigKey(cfg.getPropertyType());

        if (propertyType == "RAILROAD") {
            addTile(make_shared<RailroadTile>(
                tileIndex, cfg.getCode(), cfg.getName(),
                nullptr, OwnershipStatus::BANK,
                cfg.getPurchasePrice(), cfg.getMortgageValue()
            ));
            return;
        }
        if (propertyType == "UTILITY") {
            addTile(make_shared<UtilityTile>(
                tileIndex, cfg.getCode(), cfg.getName(),
                nullptr, OwnershipStatus::BANK,
                cfg.getPurchasePrice(), cfg.getMortgageValue()
            ));
            return;
        }
        if (propertyType != "STREET") {
            throw FileException(
                "Jenis properti tidak dikenal untuk kode " + code + ": " + cfg.getPropertyType()
            );
        }
        addTile(make_shared<StreetTile>(
            tileIndex, cfg.getCode(), cfg.getName(),
            nullptr, OwnershipStatus::BANK,
            cfg.getPurchasePrice(), cfg.getMortgageValue(),
            cfg.getColorGroup(), cfg.getHouseBuildCost(), cfg.getHotelBuildCost(),
            cfg.getRentLevels(), 0, 1, 0
        ));
    };

    auto addActionByCode = [&](const string& code, int tileIndex) {
        const ConfigManager::ActionTileConfig& cfg = configManager.getActionTileConfigByCode(code);
        const string type = this->normalizeConfigKey(cfg.getTileType());
        const string normalizedCode = this->normalizeConfigKey(cfg.getCode());

        if (type == "FESTIVAL" || type == "PETAKFESTIVAL") {
            addTile(make_shared<FestivalTile>(tileIndex, cfg.getCode(), cfg.getName()));
            return;
        }
        if (type == "KARTU" || type == "CARD" || type == "PETAKKARTU") {
            if (normalizedCode == "DNU") {
                addTile(make_shared<CommunityChestTile>(
                    tileIndex, cfg.getCode(), cfg.getName(), "COMMUNITY_CHEST"
                ));
                return;
            }
            if (normalizedCode == "KSP") {
                addTile(make_shared<ChanceTile>(tileIndex, cfg.getCode(), cfg.getName(), "CHANCE"));
                return;
            }
        }
        if (type == "PAJAK" || type == "TAX" || type == "PETAKPAJAK") {
            if (normalizedCode == "PPH") {
                addTile(make_shared<IncomeTaxTile>(
                    tileIndex, cfg.getCode(), cfg.getName(),
                    configManager.getPphFlat(), configManager.getPphFlat(),
                    configManager.getPphPercent()
                ));
                return;
            }
            if (normalizedCode == "PBM") {
                addTile(make_shared<LuxuryTaxTile>(
                    tileIndex, cfg.getCode(), cfg.getName(), configManager.getPbmFlat()
                ));
                return;
            }
        }
        if (type == "SPESIAL" || type == "SPECIAL" || type == "PETAKSPESIAL") {
            if (normalizedCode == "GO") {
                addTile(make_shared<GoTile>(
                    tileIndex, cfg.getCode(), cfg.getName(), configManager.getGoSalary()
                ));
                return;
            }
            if (normalizedCode == "PEN") {
                addTile(make_shared<JailTile>(
                    tileIndex, cfg.getCode(), cfg.getName(), configManager.getJailFine()
                ));
                return;
            }
            if (normalizedCode == "BBP") {
                addTile(make_shared<FreeParkingTile>(tileIndex, cfg.getCode(), cfg.getName()));
                return;
            }
            if (normalizedCode == "PPJ") {
                addTile(make_shared<GoToJailTile>(tileIndex, cfg.getCode(), cfg.getName()));
                return;
            }
        }
        throw FileException(
            "Konfigurasi petak aksi tidak dikenal untuk kode " +
            code + " (" + cfg.getTileType() + ")"
        );
    };

    int maxTileId = 0;
    for (const auto& entry : configManager.getPropertyCodeByIdMap()) {
        maxTileId = max(maxTileId, entry.first);
    }
    for (const auto& entry : configManager.getActionTileConfigs()) {
        maxTileId = max(maxTileId, entry.first);
    }

    for (int id = 1; id <= maxTileId; id++) {
        if (configManager.hasPropertyId(id)) {
            addPropertyByCode(configManager.getPropertyCodeById(id), id);
        } else {
            addActionByCode(configManager.getActionTileConfigById(id).getCode(), id);
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
    if (it == tiles.end()) return -1;
    return (*it)->getIndex();
}

int Board::getNextPosition(int currentPos, int steps) const {
    if (tiles.empty()) return currentPos;
    return (currentPos + steps) % static_cast<int>(tiles.size());
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

void Board::fixupOwnerPointers(const vector<Player>& oldPlayers, vector<Player>& newPlayers) {
    for (const auto& tile : tiles) {
        OwnableTile* ownable = dynamic_cast<OwnableTile*>(tile.get());
        if (ownable == nullptr || ownable->getOwner() == nullptr) continue;
        const Player* oldOwner = ownable->getOwner();
        for (size_t i = 0; i < oldPlayers.size(); ++i) {
            if (&oldPlayers[i] == oldOwner) {
                ownable->setOwner(&newPlayers[i]);
                break;
            }
        }
    }
}
