#include "controller/GUIController/GUITileController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

#include <algorithm>

using namespace view::raylibgui;

GUITileController::GUITileController(GUIGameController& controller)
    : controller_(controller) {}

int GUITileController::getMortgageValue(const TileInfo& tile) const {
    return tile.getMortgageValue() > 0 ? tile.getMortgageValue() : tile.getPrice() / 2;
}

int GUITileController::getRedeemCost(const TileInfo& tile) const {
    const int mortgageValue = getMortgageValue(tile);
    return mortgageValue + mortgageValue / 10;
}

int GUITileController::findJailIndex() const {
    for (const TileInfo& tile : controller_.state().getGame().getBoard()) {
        if (tile.getKind() == TileKind::Jail) {
            return tile.getIndex();
        }
    }
    return 6;
}

int GUITileController::computeRent(const TileInfo& tile) const {
    if (tile.isMortgaged() || tile.getOwnerIndex() < 0) {
        return 0;
    }

    if (tile.getKind() == TileKind::Street) {
        return computeStreetRent(tile);
    }

    if (tile.getKind() == TileKind::Railroad) {
        return 25 * std::max(1, countOwnedByKind(tile.getOwnerIndex(), TileKind::Railroad));
    }

    if (tile.getKind() == TileKind::Utility) {
        return countOwnedByKind(tile.getOwnerIndex(), TileKind::Utility) >= 2 ? 90 : 40;
    }

    return 0;
}

int GUITileController::computeTileAssetValue(const TileInfo& tile) const {
    int total = tile.getPrice();
    if (tile.getKind() == TileKind::Street) {
        total += std::min(tile.getBuildings(), 4) * tile.getHouseCost();
        if (tile.getBuildings() >= 5) {
            total += tile.getHotelCost();
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
    controller_.state().getGame().setSelectedTile(tileIndex);
}

void GUITileController::openTileDetail(int tileIndex) {
    controller_.state().getOverlay().setTileIndex(tileIndex);
    controller_.openOverlay(OverlayType::TileDetail);
}

void GUITileController::openPurchase(int tileIndex) {
    controller_.state().getOverlay().setTileIndex(tileIndex);
    controller_.openOverlay(OverlayType::Purchase);
}

void GUITileController::openAuctionForTile(int tileIndex) {
    AppState& state = controller_.state();
    state.getOverlay().setTileIndex(tileIndex);
    state.getOverlay().setAuction({
        tileIndex,
        state.getGame().getCurrentPlayer(),
        0,
        -1,
        std::vector<bool>(state.getGame().getPlayers().size(), false),
    });
    controller_.openOverlay(OverlayType::Auction);
}

void GUITileController::openIncomeTax() {
    controller_.state().getOverlay().setTileIndex(findFirstTile(TileKind::IncomeTax));
    controller_.openOverlay(OverlayType::IncomeTax);
}

void GUITileController::openLuxuryTax() {
    controller_.state().getOverlay().setTileIndex(findFirstTile(TileKind::LuxuryTax));
    controller_.openOverlay(OverlayType::LuxuryTax);
}

void GUITileController::openFestival() {
    controller_.state().getOverlay().setTileIndex(findFirstTile(TileKind::Festival));
    controller_.state().getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Festival);
}

void GUITileController::openJail() {
    controller_.state().getOverlay().setTileIndex(findJailIndex());
    controller_.openOverlay(OverlayType::Jail);
}

void GUITileController::openBuild() {
    controller_.state().getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Build);
}

void GUITileController::openMortgage() {
    controller_.state().getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Mortgage);
}

void GUITileController::openRedeem() {
    controller_.state().getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Redeem);
}

void GUITileController::openLiquidation() {
    controller_.state().getOverlay().setSelectedIndex(0);
    controller_.state().getOverlay().setSelectedPlayer(controller_.state().getGame().getCurrentPlayer());
    controller_.openOverlay(OverlayType::Liquidation);
}

void GUITileController::triggerTileEvent(int tileIndex, bool fromMovement) {
    (void)fromMovement;
    controller_.state().getGame().setSelectedTile(tileIndex);
}

bool GUITileController::canCurrentPlayerAffordSelectedPurchase() const {
    const GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return false;
    }

    return game.getPlayers().at(game.getCurrentPlayer()).getMoney() >= currentPurchasePrice();
}

int GUITileController::currentPurchasePrice() const {
    const AppState& state = controller_.state();
    const TileInfo& tile = state.getGame().getBoard().at(state.getOverlay().getTileIndex());
    const PlayerInfo& player = state.getGame().getPlayers().at(state.getGame().getCurrentPlayer());
    const int discount = player.getDiscountPercent();
    return discount > 0 ? tile.getPrice() * (100 - discount) / 100 : tile.getPrice();
}

void GUITileController::buySelectedProperty() {}

void GUITileController::skipSelectedPurchase() {
    controller_.closeOverlay();
}

void GUITileController::auctionRaiseBid(int amount) {
    AuctionState& auction = controller_.state().getOverlay().getAuction();
    auction.setHighestBid(auction.getHighestBid() + amount);
    auction.setHighestBidder(auction.getSelectedBidder());
}

void GUITileController::auctionPass() {
    AuctionState& auction = controller_.state().getOverlay().getAuction();
    if (auction.getSelectedBidder() < static_cast<int>(auction.getPassed().size())) {
        auction.getPassed().at(auction.getSelectedBidder()) = true;
    }
}

void GUITileController::finalizeAuction() {
    controller_.closeOverlay();
}

int GUITileController::flatIncomeTax() const {
    return 200;
}

int GUITileController::percentageIncomeTax() const {
    return std::max(120, controller_.computeNetWorth(controller_.state().getGame().getCurrentPlayer()) / 10);
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
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    game.getPlayers().at(game.getCurrentPlayer()).setBankrupt(true);
}

std::vector<int> GUITileController::currentPlayerStreetOptions() const {
    std::vector<int> streets;
    const GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return streets;
    }

    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    for (int tileIndex : player.getProperties()) {
        if (game.getBoard().at(tileIndex).getKind() == TileKind::Street) {
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
    const GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return items;
    }

    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    for (int tileIndex : player.getProperties()) {
        if (!game.getBoard().at(tileIndex).isMortgaged()) {
            items.push_back(tileIndex);
        }
    }
    return items;
}

std::vector<int> GUITileController::currentPlayerRedeemOptions() const {
    std::vector<int> items;
    const GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return items;
    }

    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    for (int tileIndex : player.getProperties()) {
        if (game.getBoard().at(tileIndex).isMortgaged()) {
            items.push_back(tileIndex);
        }
    }
    return items;
}

int GUITileController::countOwnedByKind(int ownerIndex, TileKind kind) const {
    int total = 0;
    for (const TileInfo& tile : controller_.state().getGame().getBoard()) {
        if (tile.getOwnerIndex() == ownerIndex && tile.getKind() == kind) {
            total++;
        }
    }
    return total;
}

int GUITileController::computeStreetRent(const TileInfo& tile) const {
    int rent = tile.getBaseRent();
    if (tile.getBuildings() == 1) rent = tile.getBaseRent() * 2;
    if (tile.getBuildings() == 2) rent = tile.getBaseRent() * 4;
    if (tile.getBuildings() == 3) rent = tile.getBaseRent() * 7;
    if (tile.getBuildings() == 4) rent = tile.getBaseRent() * 10;
    if (tile.getBuildings() >= 5) rent = tile.getBaseRent() * 14;
    return tile.getFestivalTurns() > 0 ? rent + rent / 2 : rent;
}

void GUITileController::removePropertyReference(PlayerInfo& player, int tileIndex) {
    player.getProperties().erase(
        std::remove(player.getProperties().begin(), player.getProperties().end(), tileIndex),
        player.getProperties().end()
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
    for (const TileInfo& tile : controller_.state().getGame().getBoard()) {
        if (tile.getKind() == kind) {
            return tile.getIndex();
        }
    }
    return -1;
}

int GUITileController::findFirstEligiblePurchaseTile() const {
    for (const TileInfo& tile : controller_.state().getGame().getBoard()) {
        if (tile.getOwnerIndex() < 0 && tile.getKind() == TileKind::Street) {
            return tile.getIndex();
        }
    }
    return -1;
}

void GUITileController::tickFestivalEffects() {}

void GUITileController::giveGoBonus(int playerIndex) {
    GameState& game = controller_.state().getGame();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(game.getPlayers().size())) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(playerIndex);
    player.setMoney(player.getMoney() + 200);
}

void GUITileController::sendCurrentPlayerToJail(const std::string& reason) {
    (void)reason;
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    player.setJailed(true);
    player.setPosition(findJailIndex());
}
