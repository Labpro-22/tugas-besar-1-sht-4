#include "controller/GUIController/GUITileController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

#include <algorithm>
#include <string>

namespace {
using namespace view::raylibgui;

bool isOwnable(TileKind kind) {
    return kind == TileKind::Street || kind == TileKind::Railroad || kind == TileKind::Utility;
}

std::vector<int> findGroupTiles(const GameState& game, const TileInfo& sourceTile) {
    std::vector<int> groupTiles;
    for (const TileInfo& tile : game.getBoard()) {
        if (tile.getKind() == TileKind::Street && tile.getGroup() == sourceTile.getGroup()) {
            groupTiles.push_back(tile.getIndex());
        }
    }
    return groupTiles;
}

bool playerOwnsFullStreetGroup(const GameState& game, int playerIndex, const TileInfo& tile) {
    if (tile.getKind() != TileKind::Street || tile.getGroup().empty()) {
        return false;
    }

    const std::vector<int> groupTiles = findGroupTiles(game, tile);
    if (groupTiles.empty()) {
        return false;
    }

    for (int tileIndex : groupTiles) {
        const TileInfo& groupedTile = game.getBoard().at(tileIndex);
        if (groupedTile.getOwnerIndex() != playerIndex || groupedTile.isMortgaged()) {
            return false;
        }
    }
    return true;
}

int recoveredBuildingValue(const TileInfo& tile) {
    if (tile.getKind() != TileKind::Street || tile.getBuildings() <= 0) {
        return 0;
    }

    const int houseCount = std::min(tile.getBuildings(), 4);
    int recovery = houseCount * tile.getHouseCost();
    if (tile.getBuildings() >= 5) {
        recovery += tile.getHotelCost();
    }
    return recovery / 2;
}
}

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
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    if (game.getPlayers().empty() || tileIndex < 0 || tileIndex >= static_cast<int>(game.getBoard().size())) {
        return;
    }

    game.setSelectedTile(tileIndex);
    TileInfo& tile = game.getBoard().at(tileIndex);
    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());

    switch (tile.getKind()) {
        case TileKind::Street:
        case TileKind::Railroad:
        case TileKind::Utility: {
            if (tile.getOwnerIndex() < 0) {
                openPurchase(tileIndex);
                return;
            }
            if (tile.getOwnerIndex() == game.getCurrentPlayer()) {
                controller_.addToast("Mendarat di properti sendiri.", tile.getAccent());
                return;
            }

            const int rent = computeRent(tile);
            if (rent <= 0) {
                controller_.addToast("Properti ini tidak menarik sewa.", tile.getAccent());
                return;
            }

            PlayerInfo& owner = game.getPlayers().at(tile.getOwnerIndex());
            owner.setMoney(owner.getMoney() + rent);
            player.setMoney(player.getMoney() - rent);
            controller_.addLog(player.getName(), "Bayar Rent", "Membayar " + std::to_string(rent) + " ke " + owner.getName() + ".");
            controller_.addToast("Bayar rent " + std::to_string(rent) + ".", tile.getAccent());
            controller_.maybeOpenLiquidation();
            return;
        }
        case TileKind::Chance:
            controller_.openRandomCardDraw(kChanceDeckKey);
            return;
        case TileKind::CommunityChest:
            controller_.openRandomCardDraw(kCommunityDeckKey);
            return;
        case TileKind::IncomeTax:
            openIncomeTax();
            return;
        case TileKind::LuxuryTax:
            openLuxuryTax();
            return;
        case TileKind::Festival:
            openFestival();
            return;
        case TileKind::GoToJail:
            sendCurrentPlayerToJail(fromMovement ? "Kartu atau perpindahan" : "Petak Go To Jail");
            controller_.openJail();
            return;
        case TileKind::Go:
            controller_.addToast("Mendarat di GO.", tile.getAccent());
            return;
        case TileKind::Jail:
            controller_.addToast(player.isJailed() ? "Masih berada di jail." : "Hanya berkunjung ke jail.", tile.getAccent());
            return;
        case TileKind::FreeParking:
            controller_.addToast("Istirahat sejenak di Free Parking.", tile.getAccent());
            return;
    }
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

void GUITileController::buySelectedProperty() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    const int tileIndex = state.getOverlay().getTileIndex();
    if (tileIndex < 0 || tileIndex >= static_cast<int>(game.getBoard().size())) {
        return;
    }

    TileInfo& tile = game.getBoard().at(tileIndex);
    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (!isOwnable(tile.getKind()) || tile.getOwnerIndex() >= 0) {
        controller_.addToast("Properti ini tidak bisa dibeli.", RED);
        return;
    }

    const int price = currentPurchasePrice();
    if (player.getMoney() < price) {
        controller_.addToast("Saldo tidak cukup untuk membeli properti.", RED);
        return;
    }

    player.setMoney(player.getMoney() - price);
    assignTileToPlayer(tileIndex, game.getCurrentPlayer());
    player.setDiscountPercent(0);
    controller_.addLog(player.getName(), "Beli Properti", "Membeli " + tile.getName() + " seharga " + std::to_string(price) + ".");
    controller_.addToast("Berhasil membeli " + tile.getName() + ".", tile.getAccent());
    controller_.closeOverlay();
}

void GUITileController::skipSelectedPurchase() {
    controller_.closeOverlay();
}

void GUITileController::auctionRaiseBid(int amount) {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    AuctionState& auction = state.getOverlay().getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(game.getPlayers().size())) {
        return;
    }
    if (bidderIndex < static_cast<int>(auction.getPassed().size()) && auction.getPassed().at(bidderIndex)) {
        controller_.addToast("Bidder ini sudah pass.", RED);
        return;
    }

    PlayerInfo& bidder = game.getPlayers().at(bidderIndex);
    if (bidder.isBankrupt()) {
        controller_.addToast("Pemain bangkrut tidak bisa ikut bid.", RED);
        return;
    }

    const int nextBid = auction.getHighestBid() + amount;
    if (bidder.getMoney() < nextBid) {
        controller_.addToast("Saldo bidder tidak cukup.", RED);
        return;
    }

    auction.setHighestBid(nextBid);
    auction.setHighestBidder(bidderIndex);
    controller_.addLog(bidder.getName(), "Auction Bid", "Menaikkan bid menjadi " + std::to_string(nextBid) + ".");
    controller_.addToast("Bid " + std::to_string(nextBid) + " oleh " + bidder.getName() + ".", bidder.getAccent());

    for (int checked = 0; checked < static_cast<int>(game.getPlayers().size()); checked++) {
        const int nextIndex = (bidderIndex + checked + 1) % static_cast<int>(game.getPlayers().size());
        if (nextIndex >= static_cast<int>(auction.getPassed().size())) {
            continue;
        }
        if (!auction.getPassed().at(nextIndex) && !game.getPlayers().at(nextIndex).isBankrupt()) {
            auction.setSelectedBidder(nextIndex);
            break;
        }
    }
}

void GUITileController::auctionPass() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    AuctionState& auction = state.getOverlay().getAuction();
    if (auction.getSelectedBidder() < static_cast<int>(auction.getPassed().size())) {
        auction.getPassed().at(auction.getSelectedBidder()) = true;
        controller_.addLog(
            game.getPlayers().at(auction.getSelectedBidder()).getName(),
            "Auction Pass",
            "Memilih pass pada lelang."
        );
    }

    for (int checked = 0; checked < static_cast<int>(game.getPlayers().size()); checked++) {
        const int nextIndex = (auction.getSelectedBidder() + checked + 1) % static_cast<int>(game.getPlayers().size());
        if (nextIndex >= static_cast<int>(auction.getPassed().size())) {
            continue;
        }
        if (!auction.getPassed().at(nextIndex) && !game.getPlayers().at(nextIndex).isBankrupt()) {
            auction.setSelectedBidder(nextIndex);
            return;
        }
    }
}

void GUITileController::finalizeAuction() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    AuctionState& auction = state.getOverlay().getAuction();
    const int tileIndex = auction.getTileIndex();
    if (tileIndex >= 0 && tileIndex < static_cast<int>(game.getBoard().size()) &&
        auction.getHighestBidder() >= 0 &&
        auction.getHighestBidder() < static_cast<int>(game.getPlayers().size())) {
        TileInfo& tile = game.getBoard().at(tileIndex);
        PlayerInfo& winner = game.getPlayers().at(auction.getHighestBidder());
        if (!winner.isBankrupt() && winner.getMoney() >= auction.getHighestBid()) {
            winner.setMoney(winner.getMoney() - auction.getHighestBid());
            assignTileToPlayer(tileIndex, auction.getHighestBidder());
            controller_.addLog(
                winner.getName(),
                "Auction Win",
                "Memenangkan " + tile.getName() + " dengan bid " + std::to_string(auction.getHighestBid()) + "."
            );
            controller_.addToast("Lelang dimenangkan " + winner.getName() + ".", winner.getAccent());
        }
    }
    controller_.closeOverlay();
}

int GUITileController::flatIncomeTax() const {
    return 200;
}

int GUITileController::percentageIncomeTax() const {
    return std::max(120, controller_.computeNetWorth(controller_.state().getGame().getCurrentPlayer()) / 10);
}

void GUITileController::payIncomeTax(bool useFlatTax) {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int tax = useFlatTax ? flatIncomeTax() : percentageIncomeTax();
    player.setMoney(player.getMoney() - tax);
    controller_.addLog(player.getName(), "Bayar PPh", "Membayar pajak sebesar " + std::to_string(tax) + ".");
    controller_.addToast("PPh dibayar.", RED);
    controller_.closeOverlay();
    controller_.maybeOpenLiquidation();
}

int GUITileController::luxuryTaxAmount() const {
    return 150;
}

void GUITileController::payLuxuryTax() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int tax = luxuryTaxAmount();
    player.setMoney(player.getMoney() - tax);
    controller_.addLog(player.getName(), "Bayar PBM", "Membayar pajak mewah sebesar " + std::to_string(tax) + ".");
    controller_.addToast("PBM dibayar.", RED);
    controller_.closeOverlay();
    controller_.maybeOpenLiquidation();
}

void GUITileController::activateFestivalOnSelectedTile() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    const std::vector<int> options = currentPlayerStreetOptions();
    if (options.empty() || game.getPlayers().empty()) {
        controller_.addToast("Belum ada street untuk festival.", RED);
        return;
    }

    const int selected = std::max(0, std::min(state.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));
    tile.setFestivalTurns(3);
    controller_.addLog(game.getPlayers().at(game.getCurrentPlayer()).getName(), "Festival", "Mengaktifkan festival di " + tile.getName() + ".");
    controller_.addToast("Festival aktif di " + tile.getName() + ".", tile.getAccent());
    controller_.closeOverlay();
}

void GUITileController::payJailFine() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int fine = 100;
    if (player.getMoney() < fine) {
        controller_.addToast("Saldo tidak cukup untuk membayar denda jail.", RED);
        controller_.openLiquidation();
        return;
    }

    player.setMoney(player.getMoney() - fine);
    player.setJailed(false);
    player.setFailedJailRolls(0);
    controller_.addLog(player.getName(), "Keluar Jail", "Membayar denda sebesar 100.");
    controller_.addToast("Denda jail dibayar.", player.getAccent());
    controller_.closeOverlay();
}

void GUITileController::buildOnSelectedTile() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    const std::vector<int> options = currentPlayerBuildOptions();
    if (options.empty() || game.getPlayers().empty()) {
        controller_.addToast("Tidak ada properti yang bisa dibangun.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int selected = std::max(0, std::min(state.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));
    if (tile.getBuildings() >= 5) {
        controller_.addToast("Properti ini sudah maksimal.", RED);
        return;
    }

    const int cost = tile.getBuildings() >= 4 ? tile.getHotelCost() : tile.getHouseCost();
    if (player.getMoney() < cost) {
        controller_.addToast("Saldo tidak cukup untuk membangun.", RED);
        return;
    }

    player.setMoney(player.getMoney() - cost);
    tile.setBuildings(tile.getBuildings() + 1);
    controller_.addLog(player.getName(), "Bangun", "Menambah bangunan di " + tile.getName() + " seharga " + std::to_string(cost) + ".");
    controller_.addToast("Bangunan ditambahkan di " + tile.getName() + ".", tile.getAccent());
    controller_.closeOverlay();
}

void GUITileController::mortgageSelectedTile() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    const std::vector<int> options = currentPlayerMortgageOptions();
    if (options.empty() || game.getPlayers().empty()) {
        controller_.addToast("Tidak ada aset yang bisa digadai.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int selected = std::max(0, std::min(state.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));

    int cashIn = getMortgageValue(tile);
    if (tile.getKind() == TileKind::Street && tile.getBuildings() > 0) {
        cashIn += recoveredBuildingValue(tile);
        tile.setBuildings(0);
    }

    tile.setMortgaged(true);
    player.setMoney(player.getMoney() + cashIn);
    controller_.addLog(player.getName(), "Mortgage", "Menggadaikan " + tile.getName() + " dan menerima " + std::to_string(cashIn) + ".");
    controller_.addToast("Aset digadai.", tile.getAccent());
    controller_.closeOverlay();
}

void GUITileController::redeemSelectedTile() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    const std::vector<int> options = currentPlayerRedeemOptions();
    if (options.empty() || game.getPlayers().empty()) {
        controller_.addToast("Tidak ada aset tergadai untuk ditebus.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int selected = std::max(0, std::min(state.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));
    const int cost = getRedeemCost(tile);
    if (player.getMoney() < cost) {
        controller_.addToast("Saldo tidak cukup untuk menebus aset.", RED);
        return;
    }

    player.setMoney(player.getMoney() - cost);
    tile.setMortgaged(false);
    controller_.addLog(player.getName(), "Redeem", "Menebus " + tile.getName() + " dengan biaya " + std::to_string(cost) + ".");
    controller_.addToast("Aset berhasil ditebus.", tile.getAccent());
    controller_.closeOverlay();
}

void GUITileController::liquidateSelectedTile() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.getProperties().empty()) {
        controller_.addToast("Tidak ada aset untuk dijual.", RED);
        return;
    }

    const int selected = std::max(0, std::min(state.getOverlay().getSelectedIndex(), static_cast<int>(player.getProperties().size()) - 1));
    const int tileIndex = player.getProperties().at(selected);
    TileInfo& tile = game.getBoard().at(tileIndex);
    const int saleValue = getMortgageValue(tile) + recoveredBuildingValue(tile);
    player.setMoney(player.getMoney() + saleValue);
    controller_.addLog(player.getName(), "Liquidation", "Menjual " + tile.getName() + " dengan nilai " + std::to_string(saleValue) + ".");
    controller_.addToast("Aset dijual untuk menambah cash.", tile.getAccent());
    releaseTile(tileIndex);

    if (player.getMoney() >= 0 || player.getProperties().empty()) {
        controller_.closeOverlay();
    } else {
        state.getOverlay().setSelectedIndex(std::max(0, std::min(state.getOverlay().getSelectedIndex(), static_cast<int>(player.getProperties().size()) - 1)));
    }
}

void GUITileController::declareBankrupt() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    std::vector<int> ownedTiles = player.getProperties();
    for (int tileIndex : ownedTiles) {
        releaseTile(tileIndex);
    }
    player.getHandCards().clear();
    player.setJailed(false);
    player.setFailedJailRolls(0);
    player.setBankrupt(true);
    controller_.addLog(player.getName(), "Bankrupt", "Pemain dinyatakan bangkrut.");
    controller_.addToast(player.getName() + " bangkrut.", RED);
    controller_.closeOverlay();

    int activePlayers = 0;
    for (const PlayerInfo& candidate : game.getPlayers()) {
        if (!candidate.isBankrupt()) {
            activePlayers++;
        }
    }
    if (activePlayers <= 1) {
        controller_.openGameOver();
        return;
    }

    controller_.commands().endTurn();
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
    std::vector<int> buildable;
    const GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return buildable;
    }

    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    for (int tileIndex : player.getProperties()) {
        const TileInfo& tile = game.getBoard().at(tileIndex);
        if (tile.getKind() != TileKind::Street || tile.isMortgaged() || tile.getBuildings() >= 5) {
            continue;
        }
        if (!playerOwnsFullStreetGroup(game, game.getCurrentPlayer(), tile)) {
            continue;
        }

        const std::vector<int> groupTiles = findGroupTiles(game, tile);
        int minimumBuildings = tile.getBuildings();
        for (int groupTileIndex : groupTiles) {
            minimumBuildings = std::min(minimumBuildings, game.getBoard().at(groupTileIndex).getBuildings());
        }
        if (tile.getBuildings() <= minimumBuildings) {
            buildable.push_back(tileIndex);
        }
    }

    return buildable;
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
    if (tile.getBuildings() == 0 &&
        tile.getOwnerIndex() >= 0 &&
        playerOwnsFullStreetGroup(controller_.state().getGame(), tile.getOwnerIndex(), tile)) {
        rent *= 2;
    }
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
    GameState& game = controller_.state().getGame();
    if (tileIndex < 0 || tileIndex >= static_cast<int>(game.getBoard().size()) ||
        playerIndex < 0 || playerIndex >= static_cast<int>(game.getPlayers().size())) {
        return;
    }

    TileInfo& tile = game.getBoard().at(tileIndex);
    if (tile.getOwnerIndex() >= 0 && tile.getOwnerIndex() < static_cast<int>(game.getPlayers().size())) {
        removePropertyReference(game.getPlayers().at(tile.getOwnerIndex()), tileIndex);
    }

    tile.setOwnerIndex(playerIndex);
    tile.setMortgaged(false);
    if (std::find(game.getPlayers().at(playerIndex).getProperties().begin(), game.getPlayers().at(playerIndex).getProperties().end(), tileIndex) ==
        game.getPlayers().at(playerIndex).getProperties().end()) {
        game.getPlayers().at(playerIndex).getProperties().push_back(tileIndex);
    }
}

void GUITileController::releaseTile(int tileIndex) {
    GameState& game = controller_.state().getGame();
    if (tileIndex < 0 || tileIndex >= static_cast<int>(game.getBoard().size())) {
        return;
    }

    TileInfo& tile = game.getBoard().at(tileIndex);
    if (tile.getOwnerIndex() >= 0 && tile.getOwnerIndex() < static_cast<int>(game.getPlayers().size())) {
        removePropertyReference(game.getPlayers().at(tile.getOwnerIndex()), tileIndex);
    }

    tile.setOwnerIndex(-1);
    tile.setBuildings(0);
    tile.setFestivalTurns(0);
    tile.setMortgaged(false);
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
        if (tile.getOwnerIndex() < 0 && isOwnable(tile.getKind())) {
            return tile.getIndex();
        }
    }
    return -1;
}

void GUITileController::tickFestivalEffects() {
    for (TileInfo& tile : controller_.state().getGame().getBoard()) {
        if (tile.getFestivalTurns() > 0) {
            tile.setFestivalTurns(tile.getFestivalTurns() - 1);
        }
    }
}

void GUITileController::giveGoBonus(int playerIndex) {
    GameState& game = controller_.state().getGame();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(game.getPlayers().size())) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(playerIndex);
    player.setMoney(player.getMoney() + 200);
}

void GUITileController::sendCurrentPlayerToJail(const std::string& reason) {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.isShieldActive()) {
        player.setShieldActive(false);
        controller_.addLog(player.getName(), "Shield", "Shield aktif menahan efek jail.");
        controller_.addToast("Shield menahan efek jail.", player.getAccent());
        return;
    }

    player.setJailed(true);
    player.setPosition(findJailIndex());
    player.setFailedJailRolls(0);
    controller_.addLog(player.getName(), "Go To Jail", reason);
    controller_.addToast(player.getName() + " masuk jail.", RED);
}
