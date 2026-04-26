#include "model/managers/PropertyManager.hpp"

#include <algorithm>

#include "model/Board.hpp"
#include "model/Dice.hpp"
#include "model/Game.hpp"
#include "model/NimonException.hpp"
#include "model/Player.hpp"
#include "model/RentContext.hpp"
#include "model/managers/ConfigManager.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/UtilityTile.hpp"

using namespace std;

PropertyManager::PropertyManager() = default;

PropertyManager::PropertyManager(const PropertyManager& other) {
    (void) other;
}

PropertyManager::~PropertyManager() {}

PropertyManager& PropertyManager::operator=(const PropertyManager& other) {
    (void) other;
    return *this;
}

RentContext PropertyManager::createRentContext(
    const Board& board,
    const ConfigManager& configManager,
    const Dice& dice,
    const OwnableTile& tile
) const {
    RentContext rentContext;
    Player* owner = tile.getOwner();

    if (owner == nullptr) return rentContext;

    const StreetTile* street = dynamic_cast<const StreetTile*>(&tile);
    if (street != nullptr) {
        rentContext.setOwnerHasColorGroup(isMonopoly(board, *street));
        return rentContext;
    }

    const RailroadTile* railroad = dynamic_cast<const RailroadTile*>(&tile);
    if (railroad != nullptr) {
        int count = board.countRailroadsOwned(*owner);
        rentContext.setRailroadRent(configManager.getRailroadRent(count));
        return rentContext;
    }

    const UtilityTile* utility = dynamic_cast<const UtilityTile*>(&tile);
    if (utility != nullptr) {
        int count = board.countUtilitiesOwned(*owner);
        rentContext.setUtilityMultiplier(configManager.getUtilityMultiplier(count));
        rentContext.setDiceTotal(dice.getTotal());
    }

    return rentContext;
}

void PropertyManager::payRent(
    const Board& board,
    const ConfigManager& configManager,
    const Dice& dice,
    Player& player,
    OwnableTile& tile
) {
    Player* owner = tile.getOwner();
    if (owner == nullptr || owner == &player || tile.isMortgaged()) return;

    RentContext rentContext = createRentContext(board, configManager, dice, tile);
    int rentAmount = tile.calculateRent(rentContext);
    const int effectiveRent = player.effectiveCost(rentAmount);
    if (effectiveRent > player.getMoney()) {
        throw InsufficientFundsException(effectiveRent, player.getMoney());
    }
    player -= rentAmount;
    *owner += effectiveRent;
}

void PropertyManager::mortgageProperty(Player& player, OwnableTile& tile) {
    if (!canMortgage(player, tile)) return;
    tile.mortgage();
    player += tile.getMortgageValue();
}

int PropertyManager::getRedeemCost(const OwnableTile& tile) const {
    if (dynamic_cast<const RailroadTile*>(&tile) != nullptr ||
        dynamic_cast<const UtilityTile*>(&tile) != nullptr) {
        return tile.getMortgageValue() * 2;
    }
    return tile.getPurchasePrice();
}

void PropertyManager::redeemProperty(Player& player, OwnableTile& tile) {
    if (!tile.isMortgaged()) return;
    int redeemCost = getRedeemCost(tile);
    
    if (player.getMoney() < player.effectiveCost(redeemCost)) return;
    player -= redeemCost;
    tile.redeem();
}

void PropertyManager::buildOnStreet(const Board& board, Player& player, StreetTile& tile) {
    if (canBuildHouse(board, player, tile)) {
        tile.buildHouse();
        player -= tile.getHouseBuildCost();
    } else if (canBuildHotel(board, player, tile)) {
        tile.buildHotel();
        player -= tile.getHotelBuildCost();
    }
}

void PropertyManager::sellBuildingsInColorGroup(Player& player, const string& colorGroup) {
    (void) player;
    (void) colorGroup;
}

bool PropertyManager::canMortgage(const Player& player, const OwnableTile& tile) const {
    if (tile.isOwned() && tile.getOwner() == &player && !tile.isMortgaged()) return true;
    return false;
}

bool PropertyManager::canBuild(const Board& board, const Player& player, const StreetTile& tile) const {
    return canBuildHouse(board, player, tile) || canBuildHotel(board, player, tile);
}

bool PropertyManager::canBuildHouse(const Board& board, const Player& player, const StreetTile& tile) const {
    if (!tile.isOwned() || tile.getOwner() != &player) return false;
    if (tile.hasHotel() || tile.getBuildingLevel() == 4) return false;
    if (player.getMoney() < player.effectiveCost(tile.getHouseBuildCost())) return false;

    vector<shared_ptr<StreetTile>> streets = board.getStreetTileByColorGroup(tile.getColorGroup());
    if (streets.empty()) return false;

    if (!all_of(streets.begin(), streets.end(), [&](const shared_ptr<StreetTile>& street) {
        return street->getOwner() == &player;
    })) return false;

    if (any_of(streets.begin(), streets.end(), [](const shared_ptr<StreetTile>& street) {
        return street->isMortgaged();
    })) return false;

    int minBuildingLevel = min_element(streets.begin(), streets.end(), [](const shared_ptr<StreetTile>& first, const shared_ptr<StreetTile>& second) {
        return first->getBuildingLevel() < second->getBuildingLevel();
    })->get()->getBuildingLevel();

    return tile.getBuildingLevel() == minBuildingLevel;
}

bool PropertyManager::canBuildHotel(const Board& board, const Player& player, const StreetTile& tile) const {
    if (!tile.isOwned() || tile.getOwner() != &player) return false;
    if (tile.hasHotel()) return false;
    if (player.getMoney() < player.effectiveCost(tile.getHotelBuildCost())) return false;

    vector<shared_ptr<StreetTile>> streets = board.getStreetTileByColorGroup(tile.getColorGroup());
    if (streets.empty()) return false;

    if (!all_of(streets.begin(), streets.end(), [&](const shared_ptr<StreetTile>& street) {
        return street->getOwner() == &player;
    })) return false;

    if (any_of(streets.begin(), streets.end(), [](const shared_ptr<StreetTile>& street) {
        return street->isMortgaged();
    })) return false;

    int minBuildingLevel = min_element(streets.begin(), streets.end(), [](const shared_ptr<StreetTile>& first, const shared_ptr<StreetTile>& second) {
        return first->getBuildingLevel() < second->getBuildingLevel();
    })->get()->getBuildingLevel();

    return minBuildingLevel == 4 && tile.getBuildingLevel() == 4;
}

bool PropertyManager::isMonopoly(const Board& board, const StreetTile& tile) const {
    Player* owner = tile.getOwner();
    if (owner == nullptr) return false;

    vector<shared_ptr<StreetTile>> streets = board.getStreetTileByColorGroup(tile.getColorGroup());
    if (streets.empty()) return false;

    return all_of(streets.begin(), streets.end(), [&](const shared_ptr<StreetTile>& street) {
        return street->getOwner() == owner;
    });
}

void PropertyManager::sellPropertyToBank(Player& player, OwnableTile& tile) {
    if (!tile.isOwned() || tile.getOwner() != &player) return;
    int sellValue = calculateSellToBankValue(tile);
    player += sellValue;
    tile.setOwner(nullptr);
}

int PropertyManager::calculateSellToBankValue(const OwnableTile& tile) const {
    return tile.getSelltoBankValue();
}

vector<OwnableTile*> PropertyManager::getOwnedProperties(const Board& board, const Player& player) const {
    vector<OwnableTile*> ownedProperties;
    for (int index = 1; index <= board.getBoardSize(); index++) {
        shared_ptr<Tile> tile = board.getTile(index);
        shared_ptr<OwnableTile> property = dynamic_pointer_cast<OwnableTile>(tile);
        if (property != nullptr && property->getOwner() == &player) {
            ownedProperties.push_back(property.get());
        }
    }
    return ownedProperties;
}

int PropertyManager::countOwnedProperties(const Board& board, const Player& player) const {
    return static_cast<int>(getOwnedProperties(board, player).size());
}

int PropertyManager::calculateTotalWealth(const Board& board, const Player& player) const {
//     //TODO : hitungn total wealth dengan hitung money + harga tiap properti
    int wealth = player.getMoney();
    for (const auto& property : getOwnedProperties(board, player)) {
        if (property != nullptr) {
            wealth += property->getPurchasePrice();
        }
    }
//     // int wealth = money;
//     // for (const auto& property : ownedProperties) {
//     //     wealth += property->purchasePrice;
//     // }
//     // return wealth;
    return wealth;
}

vector<OwnableTile*> PropertyManager::getMortgageableProperties(const Game& game, const Player& player) const {
    vector<OwnableTile*> mortgageableProperties;
    for (OwnableTile* property : getOwnedProperties(game.getBoard(), player)) {
        if (property == nullptr || !canMortgage(player, *property)) {
            continue;
        }
        mortgageableProperties.push_back(property);
    }
    return mortgageableProperties;
}

vector<OwnableTile*> PropertyManager::getRedeemableProperties(const Game& game, const Player& player) const {
    vector<OwnableTile*> redeemableProperties;
    for (OwnableTile* property : getOwnedProperties(game.getBoard(), player)) {
        if (property != nullptr && property->isMortgaged()) {
            redeemableProperties.push_back(property);
        }
    }
    return redeemableProperties;
}

vector<string> PropertyManager::getBuildableColorGroups(const Game& game, const Player& player) const {
    vector<string> buildableColorGroups;
    for (const auto& property : getOwnedProperties(game.getBoard(), player)) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr && canBuild(game.getBoard(), player, *street)) {
            if (find(buildableColorGroups.begin(), buildableColorGroups.end(), street->getColorGroup()) == buildableColorGroups.end()) {
                buildableColorGroups.push_back(street->getColorGroup());
            }
        }
    }
    return buildableColorGroups;
}

vector<StreetTile*> PropertyManager::getBuildableStreets(const Game& game, const Player& player, const string& colorGroup) const {
    vector<StreetTile*> buildableStreets;
    for (OwnableTile* property : getOwnedProperties(game.getBoard(), player)) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr && canBuild(game.getBoard(), player, *street) && street->getColorGroup() == colorGroup) {
            buildableStreets.push_back(street);
        }
    }
    return buildableStreets;
}
