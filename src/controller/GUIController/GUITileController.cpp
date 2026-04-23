#include "controller/GUIController/GUITileController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

#include <algorithm>

using namespace view::raylibgui;

GUITileController::GUITileController(GUIGameController& controller)
    : controller_(controller) {}

int GUITileController::getMortgageValue(const TileInfo& tile) const {
    return tile.mortgageValue > 0 ? tile.mortgageValue : tile.price / 2;
}

int GUITileController::getRedeemCost(const TileInfo& tile) const {
    const int mortgageValue = getMortgageValue(tile);
    return mortgageValue + mortgageValue / 10;
}

int GUITileController::findJailIndex() const {
    for (const TileInfo& tile : controller_.state().game.board) {
        if (tile.kind == TileKind::Jail) {
            return tile.index;
        }
    }
    return 6;
}

int GUITileController::computeRent(const TileInfo& tile) const {
    if (tile.mortgaged || tile.ownerIndex < 0) {
        return 0;
    }

    if (tile.kind == TileKind::Street) {
        return computeStreetRent(tile);
    }

    if (tile.kind == TileKind::Railroad) {
        return 25 * std::max(1, countOwnedByKind(tile.ownerIndex, TileKind::Railroad));
    }

    if (tile.kind == TileKind::Utility) {
        return countOwnedByKind(tile.ownerIndex, TileKind::Utility) >= 2 ? 90 : 40;
    }

    return 0;
}

int GUITileController::computeTileAssetValue(const TileInfo& tile) const {
    int total = tile.price;
    if (tile.kind == TileKind::Street) {
        total += std::min(tile.buildings, 4) * tile.houseCost;
        if (tile.buildings >= 5) {
            total += tile.hotelCost;
        }
    }
    return total;
}

Rectangle GUITileController::boardTileRect(Rectangle square, int index) const {
    const float cell = square.width / static_cast<float>(kGridCells);
    int row = 0;
    int column = 0;

    if (index <= 6) {
        row = 6;
        column = 6 - index;
    } else if (index <= 12) {
        row = 12 - index;
        column = 0;
    } else if (index <= 18) {
        row = 0;
        column = index - 12;
    } else {
        row = index - 18;
        column = 6;
    }

    return {square.x + column * cell, square.y + row * cell, cell, cell};
}

void GUITileController::setSelectedTile(int tileIndex) {
    controller_.state().game.selectedTile = tileIndex;
}

void GUITileController::openTileDetail(int tileIndex) {
    controller_.state().overlay.tileIndex = tileIndex;
    controller_.openOverlay(OverlayType::TileDetail);
}

void GUITileController::openPurchase(int tileIndex) {
    controller_.state().overlay.tileIndex = tileIndex;
    controller_.openOverlay(OverlayType::Purchase);
}

void GUITileController::openAuctionForTile(int tileIndex) {
    controller_.state().overlay.tileIndex = tileIndex;
    controller_.state().overlay.auction = {
        tileIndex,
        controller_.state().game.currentPlayer,
        0,
        -1,
        std::vector<bool>(controller_.state().game.players.size(), false),
    };
    controller_.openOverlay(OverlayType::Auction);
}

void GUITileController::openIncomeTax() {
    controller_.state().overlay.tileIndex = findFirstTile(TileKind::IncomeTax);
    controller_.openOverlay(OverlayType::IncomeTax);
}

void GUITileController::openLuxuryTax() {
    controller_.state().overlay.tileIndex = findFirstTile(TileKind::LuxuryTax);
    controller_.openOverlay(OverlayType::LuxuryTax);
}

void GUITileController::openFestival() {
    controller_.state().overlay.tileIndex = findFirstTile(TileKind::Festival);
    controller_.state().overlay.selectedIndex = 0;
    controller_.openOverlay(OverlayType::Festival);
}

void GUITileController::openJail() {
    controller_.state().overlay.tileIndex = findJailIndex();
    controller_.openOverlay(OverlayType::Jail);
}

void GUITileController::openBuild() {
    controller_.state().overlay.selectedIndex = 0;
    controller_.openOverlay(OverlayType::Build);
}

void GUITileController::openMortgage() {
    controller_.state().overlay.selectedIndex = 0;
    controller_.openOverlay(OverlayType::Mortgage);
}

void GUITileController::openRedeem() {
    controller_.state().overlay.selectedIndex = 0;
    controller_.openOverlay(OverlayType::Redeem);
}

void GUITileController::openLiquidation() {
    controller_.state().overlay.selectedIndex = 0;
    controller_.state().overlay.selectedPlayer = controller_.state().game.currentPlayer;
    controller_.openOverlay(OverlayType::Liquidation);
}

void GUITileController::triggerTileEvent(int tileIndex, bool fromMovement) {
    (void)fromMovement;
    controller_.state().game.selectedTile = tileIndex;
}

bool GUITileController::canCurrentPlayerAffordSelectedPurchase() const {
    if (controller_.state().game.players.empty()) {
        return false;
    }
    return controller_.state().game.players.at(controller_.state().game.currentPlayer).money >= currentPurchasePrice();
}

int GUITileController::currentPurchasePrice() const {
    const TileInfo& tile = controller_.state().game.board.at(controller_.state().overlay.tileIndex);
    const PlayerInfo& player = controller_.state().game.players.at(controller_.state().game.currentPlayer);
    const int discount = player.discountPercent;
    return discount > 0 ? tile.price * (100 - discount) / 100 : tile.price;
}

void GUITileController::buySelectedProperty() {}

void GUITileController::skipSelectedPurchase() {
    controller_.closeOverlay();
}

void GUITileController::auctionRaiseBid(int amount) {
    controller_.state().overlay.auction.highestBid += amount;
    controller_.state().overlay.auction.highestBidder = controller_.state().overlay.auction.selectedBidder;
}

void GUITileController::auctionPass() {
    if (controller_.state().overlay.auction.selectedBidder < static_cast<int>(controller_.state().overlay.auction.passed.size())) {
        controller_.state().overlay.auction.passed.at(controller_.state().overlay.auction.selectedBidder) = true;
    }
}

void GUITileController::finalizeAuction() {
    controller_.closeOverlay();
}

int GUITileController::flatIncomeTax() const {
    return 200;
}

int GUITileController::percentageIncomeTax() const {
    return std::max(120, controller_.computeNetWorth(controller_.state().game.currentPlayer) / 10);
}

void GUITileController::payIncomeTax(bool useFlatTax) {
    (void)useFlatTax;
    controller_.closeOverlay();
}

int GUITileController::luxuryTaxAmount() const {
    return 150;
}

void GUITileController::payLuxuryTax() {
    controller_.closeOverlay();
}

void GUITileController::activateFestivalOnSelectedTile() {}

void GUITileController::payJailFine() {
    controller_.closeOverlay();
}

void GUITileController::buildOnSelectedTile() {}

void GUITileController::mortgageSelectedTile() {}

void GUITileController::redeemSelectedTile() {}

void GUITileController::liquidateSelectedTile() {}

void GUITileController::declareBankrupt() {
    if (controller_.state().game.players.empty()) {
        return;
    }
    controller_.state().game.players.at(controller_.state().game.currentPlayer).bankrupt = true;
}

std::vector<int> GUITileController::currentPlayerStreetOptions() const {
    std::vector<int> streets;
    if (controller_.state().game.players.empty()) {
        return streets;
    }

    const PlayerInfo& player = controller_.state().game.players.at(controller_.state().game.currentPlayer);
    for (int tileIndex : player.properties) {
        if (controller_.state().game.board.at(tileIndex).kind == TileKind::Street) {
            streets.push_back(tileIndex);
        }
    }
    return streets;
}

std::vector<int> GUITileController::currentPlayerBuildOptions() const {
    return currentPlayerStreetOptions();
}

std::vector<int> GUITileController::currentPlayerMortgageOptions() const {
    std::vector<int> items;
    if (controller_.state().game.players.empty()) {
        return items;
    }

    const PlayerInfo& player = controller_.state().game.players.at(controller_.state().game.currentPlayer);
    for (int tileIndex : player.properties) {
        if (!controller_.state().game.board.at(tileIndex).mortgaged) {
            items.push_back(tileIndex);
        }
    }
    return items;
}

std::vector<int> GUITileController::currentPlayerRedeemOptions() const {
    std::vector<int> items;
    if (controller_.state().game.players.empty()) {
        return items;
    }

    const PlayerInfo& player = controller_.state().game.players.at(controller_.state().game.currentPlayer);
    for (int tileIndex : player.properties) {
        if (controller_.state().game.board.at(tileIndex).mortgaged) {
            items.push_back(tileIndex);
        }
    }
    return items;
}

int GUITileController::countOwnedByKind(int ownerIndex, TileKind kind) const {
    int total = 0;
    for (const TileInfo& tile : controller_.state().game.board) {
        if (tile.ownerIndex == ownerIndex && tile.kind == kind) {
            total++;
        }
    }
    return total;
}

int GUITileController::computeStreetRent(const TileInfo& tile) const {
    int rent = tile.baseRent;
    if (tile.buildings == 1) rent = tile.baseRent * 2;
    if (tile.buildings == 2) rent = tile.baseRent * 4;
    if (tile.buildings == 3) rent = tile.baseRent * 7;
    if (tile.buildings == 4) rent = tile.baseRent * 10;
    if (tile.buildings >= 5) rent = tile.baseRent * 14;
    return tile.festivalTurns > 0 ? rent + rent / 2 : rent;
}

void GUITileController::removePropertyReference(PlayerInfo& player, int tileIndex) {
    player.properties.erase(
        std::remove(player.properties.begin(), player.properties.end(), tileIndex),
        player.properties.end()
    );
}

void GUITileController::assignTileToPlayer(int tileIndex, int playerIndex) {
    (void)tileIndex;
    (void)playerIndex;
}

void GUITileController::releaseTile(int tileIndex) {
    (void)tileIndex;
}

int GUITileController::findFirstTile(TileKind kind) const {
    for (const TileInfo& tile : controller_.state().game.board) {
        if (tile.kind == kind) {
            return tile.index;
        }
    }
    return -1;
}

int GUITileController::findFirstEligiblePurchaseTile() const {
    for (const TileInfo& tile : controller_.state().game.board) {
        if (tile.ownerIndex < 0 && tile.kind == TileKind::Street) {
            return tile.index;
        }
    }
    return -1;
}

void GUITileController::tickFestivalEffects() {}

void GUITileController::giveGoBonus(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.state().game.players.size())) {
        return;
    }
    controller_.state().game.players.at(playerIndex).money += 200;
}

void GUITileController::sendCurrentPlayerToJail(const std::string& reason) {
    (void)reason;
    if (controller_.state().game.players.empty()) {
        return;
    }
    PlayerInfo& player = controller_.state().game.players.at(controller_.state().game.currentPlayer);
    player.jailed = true;
    player.position = findJailIndex();
}
