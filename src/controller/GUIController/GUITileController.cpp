#include "controller/GUIController/GUITileController.hpp"

#include "model/Game.hpp"
#include "model/RentContext.hpp"
#include "model/tiles/FestivalTile.hpp"
#include "model/tiles/GoTile.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/Tile.hpp"
#include "model/tiles/UtilityTile.hpp"
#include "view/raylib/UiToolkit.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <memory>
#include <string>
#include <vector>

using namespace view::raylibgui;

namespace {
UiToolkit tileToolkit;

Color playerAccent(int index) {
    switch (index) {
        case 0: return tileToolkit.theme().getCoral();
        case 1: return tileToolkit.theme().getTeal();
        case 2: return tileToolkit.theme().getGold();
        default: return tileToolkit.theme().getNavy();
    }
}

std::string normalizeKey(std::string value) {
    std::string normalized;
    for (char ch : value) {
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
    }
    return normalized;
}

Color groupAccent(const std::string& group) {
    const std::string key = normalizeKey(group);
    if (key == "COKLAT" || key == "BROWN") return {142, 96, 70, 255};
    if (key == "BIRUMUDA" || key == "LIGHTBLUE") return {86, 165, 197, 255};
    if (key == "PINK") return {205, 105, 149, 255};
    if (key == "ORANGE" || key == "ORANYE") return {226, 127, 59, 255};
    if (key == "MERAH" || key == "RED") return {190, 76, 71, 255};
    if (key == "KUNING" || key == "YELLOW") return {220, 176, 78, 255};
    if (key == "HIJAU" || key == "GREEN") return {96, 154, 101, 255};
    if (key == "BIRUTUA" || key == "NAVY" || key == "DARKBLUE") return {58, 73, 111, 255};
    if (key == "ABUABU" || key == "UTILITY") return tileToolkit.theme().getTeal();
    return tileToolkit.colorForGroup(group);
}

Color kindAccent(TileKind kind, const std::string& group) {
    if (kind == TileKind::Street) return groupAccent(group);
    if (kind == TileKind::Railroad) return tileToolkit.theme().getNavy();
    if (kind == TileKind::Utility) return tileToolkit.theme().getTeal();
    if (kind == TileKind::Chance) return tileToolkit.theme().getCoral();
    if (kind == TileKind::CommunityChest) return tileToolkit.theme().getSage();
    if (kind == TileKind::IncomeTax || kind == TileKind::LuxuryTax) return tileToolkit.theme().getDanger();
    if (kind == TileKind::Festival) return tileToolkit.theme().getGold();
    if (kind == TileKind::Jail || kind == TileKind::GoToJail) return tileToolkit.theme().getDanger();
    if (kind == TileKind::Go) return tileToolkit.theme().getGold();
    return tileToolkit.theme().getPaper();
}
}

GUITileController::GUITileController(GUIControllerContext& controller)
    : controller_(controller) {
    controller_.openJailCallback = [this]() { openJail(); };
    controller_.moveBackendPlayerCallback = [this](Player& player, int steps) { return moveBackendPlayer(player, steps); };
    controller_.resolveBackendLandingCallback = [this](int backendTileIndex, bool fromMovement) {
        resolveBackendLanding(backendTileIndex, fromMovement);
    };
}

int GUITileController::getMortgageValue(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    return ownable != nullptr ? ownable->getMortgageValue() : tile.getMortgageValue();
}

int GUITileController::getRedeemCost(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    return ownable != nullptr ? controller_.backendGame_.getPropertyManager().getRedeemCost(*ownable) : getMortgageValue(tile);
}

int GUITileController::findJailIndex() const {
    for (const std::shared_ptr<Tile>& tile : controller_.backendGame_.getBoard().getTiles()) {
        if (tile != nullptr && tile->onLand() == Tile::TileType::Jail) {
            return controller_.uiTileIndexFromBackend(tile->getIndex());
        }
    }
    return 0;
}

int GUITileController::computeRent(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    if (ownable == nullptr || ownable->getOwner() == nullptr || ownable->isMortgaged()) {
        return 0;
    }

    RentContext rentContext = controller_.backendGame_.getPropertyManager().createRentContext(
        controller_.backendGame_.getBoard(),
        controller_.backendGame_.getConfigManager(),
        controller_.backendGame_.getDice(),
        *ownable
    );
    return ownable->calculateRent(rentContext);
}

int GUITileController::computeTileAssetValue(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    if (ownable == nullptr) {
        return 0;
    }
    return ownable->getPurchasePrice() + ownable->getBuildingValue();
}

int GUITileController::computeNetWorth(int playerIndex) const {
    const std::vector<Player>& players = controller_.backendGame_.getPlayers();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return 0;
    }

    int total = players.at(playerIndex).getMoney();
    for (OwnableTile* property : controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), players.at(playerIndex))) {
        if (property != nullptr) {
            total += property->getPurchasePrice() + property->getBuildingValue();
        }
    }
    total += players.at(playerIndex).countCards() * 75;
    return total;
}

int GUITileController::jailFineAmount() const {
    return controller_.backendGame_.getConfigManager().getJailFine();
}

Rectangle GUITileController::boardTileRect(Rectangle square, int index) const {
    const int edge = kGridCells - 1;
    const int perimeter = edge * 4;
    const int normalized = perimeter > 0 ? ((index % perimeter) + perimeter) % perimeter : index;
    const float cell = square.width / static_cast<float>(kGridCells);

    int row = 0;
    int column = 0;
    if (normalized <= edge) {
        row = edge;
        column = edge - normalized;
    } else if (normalized <= edge * 2) {
        row = edge * 2 - normalized;
        column = 0;
    } else if (normalized <= edge * 3) {
        row = 0;
        column = normalized - edge * 2;
    } else {
        row = normalized - edge * 3;
        column = edge;
    }

    return {square.x + column * cell, square.y + row * cell, cell, cell};
}

void GUITileController::setSelectedTile(int tileIndex) {
    const int boardSize = static_cast<int>(controller_.appState_.getGame().getBoard().size());
    if (boardSize <= 0) {
        controller_.appState_.getGame().setSelectedTile(0);
        return;
    }
    controller_.appState_.getGame().setSelectedTile(std::max(0, std::min(tileIndex, boardSize - 1)));
}

void GUITileController::openTileDetail(int tileIndex) {
    controller_.appState_.getOverlay().setTileIndex(tileIndex);
    controller_.openOverlay(OverlayType::TileDetail);
}

void GUITileController::openPurchase(int tileIndex) {
    controller_.appState_.getOverlay().setTileIndex(tileIndex);
    controller_.openOverlay(OverlayType::Purchase);
}

void GUITileController::openAuctionForTile(int tileIndex) {
    controller_.appState_.getOverlay().setTileIndex(tileIndex);

    if (StreetTile* street = controller_.streetFromUi(tileIndex)) {
        try {
            controller_.backendGame_.getAuctionManager().initializeAuction(
                controller_.backendGame_.getGameContext(),
                *street,
                controller_.backendGame_.getPlayers().empty() ? nullptr : &controller_.backendGame_.getCurrentPlayer()
            );
        } catch (const std::exception& exception) {
            controller_.addToast(exception.what(), RED);
        }
    }

    controller_.appState_.getOverlay().setAuction({
        tileIndex,
        controller_.appState_.getGame().getCurrentPlayer(),
        0,
        -1,
        std::vector<bool>(controller_.appState_.getGame().getPlayers().size(), false),
    });
    controller_.openOverlay(OverlayType::Auction);
}

void GUITileController::openIncomeTax() {
    for (const TileInfo& tile : controller_.appState_.getGame().getBoard()) {
        if (tile.getKind() == TileKind::IncomeTax) {
            controller_.appState_.getOverlay().setTileIndex(tile.getIndex());
            break;
        }
    }
    controller_.openOverlay(OverlayType::IncomeTax);
}

void GUITileController::openLuxuryTax() {
    for (const TileInfo& tile : controller_.appState_.getGame().getBoard()) {
        if (tile.getKind() == TileKind::LuxuryTax) {
            controller_.appState_.getOverlay().setTileIndex(tile.getIndex());
            break;
        }
    }
    controller_.openOverlay(OverlayType::LuxuryTax);
}

void GUITileController::openFestival() {
    for (const TileInfo& tile : controller_.appState_.getGame().getBoard()) {
        if (tile.getKind() == TileKind::Festival) {
            controller_.appState_.getOverlay().setTileIndex(tile.getIndex());
            break;
        }
    }
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Festival);
}

void GUITileController::openJail() {
    controller_.appState_.getOverlay().setTileIndex(findJailIndex());
    controller_.openOverlay(OverlayType::Jail);
}

void GUITileController::openBuild() {
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Build);
}

void GUITileController::openMortgage() {
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Mortgage);
}

void GUITileController::openRedeem() {
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Redeem);
}

void GUITileController::openLiquidation() {
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.appState_.getOverlay().setSelectedPlayer(controller_.appState_.getGame().getCurrentPlayer());
    controller_.openOverlay(OverlayType::Liquidation);
}

void GUITileController::triggerTileEvent(int tileIndex, bool fromMovement) {
    resolveBackendLanding(controller_.backendTileIndexFromUi(tileIndex), fromMovement);
}

bool GUITileController::canCurrentPlayerAffordSelectedPurchase() const {
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return false;
    }
    return controller_.backendGame_.getPlayers().at(playerIndex).getMoney() >= currentPurchasePrice();
}

int GUITileController::currentPurchasePrice() const {
    const OwnableTile* ownable = controller_.ownableFromUi(controller_.appState_.getOverlay().getTileIndex());
    if (ownable == nullptr) {
        return 0;
    }
    return controller_.backendGame_.getPlayers().at(controller_.currentBackendPlayerIndex()).effectiveCost(ownable->getPurchasePrice());
}

void GUITileController::buySelectedProperty() {
    try {
        OwnableTile* ownable = controller_.ownableFromUi(controller_.appState_.getOverlay().getTileIndex());
        Player& player = controller_.backendGame_.getCurrentPlayer();
        if (ownable == nullptr) {
            controller_.addToast("Petak ini tidak bisa dibeli.", RED);
            return;
        }
        if (ownable->getOwner() != nullptr && ownable->getOwnershipStatus() != OwnershipStatus::BANK) {
            controller_.addToast("Properti sudah dimiliki.", RED);
            return;
        }

        const int price = player.effectiveCost(ownable->getPurchasePrice());
        if (player.getMoney() < price) {
            controller_.addToast("Uang tidak cukup.", RED);
            return;
        }

        player -= ownable->getPurchasePrice();
        ownable->setOwner(&player);
        ownable->setOwnershipStatus(OwnershipStatus::OWNED);
        controller_.addLog(player.getUsername(), "BELI", "Membeli " + ownable->getName() + " seharga M" + std::to_string(price) + ".");
        controller_.addToast("Properti dibeli.", kindAccent(controller_.toGuiTileKind(*ownable), ""));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::skipSelectedPurchase() {
    const int tileIndex = controller_.appState_.getOverlay().getTileIndex();
    controller_.closeOverlay();
    openAuctionForTile(tileIndex);
}

void GUITileController::auctionRaiseBid(int amount) {
    OverlayState& overlay = controller_.appState_.getOverlay();
    AuctionState auction = overlay.getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return;
    }

    const int nextBid = std::max(auction.getHighestBid() + amount, auction.getHighestBid() + 1);
    Player& bidder = controller_.backendGame_.getPlayers().at(bidderIndex);
    if (bidder.getMoney() < nextBid) {
        controller_.addToast("Bidder tidak punya uang cukup.", RED);
        return;
    }

    controller_.backendGame_.getAuctionManager().placeBid(bidder, nextBid);
    auction.setHighestBid(nextBid);
    auction.setHighestBidder(bidderIndex);

    int nextActive = bidderIndex;
    for (int checked = 0; checked < static_cast<int>(controller_.backendGame_.getPlayers().size()); checked++) {
        const int nextIndex = (bidderIndex + checked + 1) % static_cast<int>(controller_.backendGame_.getPlayers().size());
        if (nextIndex != bidderIndex &&
            !auction.getPassed().at(nextIndex) &&
            !controller_.backendGame_.getPlayers().at(nextIndex).isBankrupt()) {
            nextActive = nextIndex;
            break;
        }
    }
    auction.setSelectedBidder(nextActive);
    overlay.setAuction(auction);
    controller_.addLog(bidder.getUsername(), "BID", "Menawar M" + std::to_string(nextBid) + ".");
    controller_.syncViewFromBackend();
}

void GUITileController::auctionPass() {
    OverlayState& overlay = controller_.appState_.getOverlay();
    AuctionState auction = overlay.getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return;
    }

    std::vector<bool> passed = auction.getPassed();
    passed.at(bidderIndex) = true;
    auction.setPassed(passed);
    controller_.backendGame_.getAuctionManager().pass(controller_.backendGame_.getPlayers().at(bidderIndex));
    controller_.addLog(controller_.backendGame_.getPlayers().at(bidderIndex).getUsername(), "PASS", "Pass pada lelang.");

    int nextActive = bidderIndex;
    for (int checked = 0; checked < static_cast<int>(controller_.backendGame_.getPlayers().size()); checked++) {
        const int nextIndex = (bidderIndex + checked + 1) % static_cast<int>(controller_.backendGame_.getPlayers().size());
        if (!passed.at(nextIndex) &&
            !controller_.backendGame_.getPlayers().at(nextIndex).isBankrupt()) {
            nextActive = nextIndex;
            break;
        }
    }
    auction.setSelectedBidder(nextActive);
    overlay.setAuction(auction);
    finalizeAuction();
}

void GUITileController::finalizeAuction() {
    OverlayState& overlay = controller_.appState_.getOverlay();
    AuctionState auction = overlay.getAuction();
    const int remaining = static_cast<int>(std::count(auction.getPassed().begin(), auction.getPassed().end(), false));
    if (remaining > 1) {
        return;
    }

    OwnableTile* ownable = controller_.ownableFromUi(auction.getTileIndex());
    if (ownable != nullptr && auction.getHighestBidder() >= 0) {
        const int winnerIndex = auction.getHighestBidder();
        if (winnerIndex >= 0 && winnerIndex < static_cast<int>(controller_.backendGame_.getPlayers().size())) {
            Player& winner = controller_.backendGame_.getPlayers().at(winnerIndex);
            winner -= auction.getHighestBid();
            ownable->setOwner(&winner);
            ownable->setOwnershipStatus(OwnershipStatus::OWNED);
            controller_.addLog(winner.getUsername(), "MENANG_LELANG", "Menang " + ownable->getName() + " seharga M" + std::to_string(auction.getHighestBid()) + ".");
            controller_.addToast("Lelang dimenangkan " + winner.getUsername() + ".", kindAccent(controller_.toGuiTileKind(*ownable), ""));
        }
    } else if (ownable != nullptr) {
        ownable->setOwner(nullptr);
        ownable->setOwnershipStatus(OwnershipStatus::BANK);
        controller_.addToast("Tidak ada pemenang lelang.", RED);
    }

    controller_.closeOverlay();
    controller_.syncViewFromBackend();
}

int GUITileController::flatIncomeTax() const {
    return controller_.backendGame_.getConfigManager().getPphFlat();
}

int GUITileController::percentageIncomeTax() const {
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return 0;
    }
    const int wealth = computeNetWorth(playerIndex);
    return wealth * controller_.backendGame_.getConfigManager().getPphPercent() / 100;
}

void GUITileController::payIncomeTax(bool useFlatTax) {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        const int tax = useFlatTax ? flatIncomeTax() : percentageIncomeTax();
        if (player.isShieldActive()) {
            player.setShieldActive(false);
            controller_.addToast("Shield menahan pajak.", playerAccent(controller_.currentBackendPlayerIndex()));
        } else if (player.getMoney() < player.effectiveCost(tax)) {
            controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, tax, true);
            controller_.addToast("Dana kurang untuk membayar pajak.", RED);
        } else {
            controller_.backendGame_.getTaxManager().processTaxPayment(player, tax);
        }
        controller_.addLog(player.getUsername(), "PAJAK", "Membayar PPh M" + std::to_string(tax) + ".");
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
    }
}

int GUITileController::luxuryTaxAmount() const {
    return controller_.backendGame_.getConfigManager().getPbmFlat();
}

void GUITileController::payLuxuryTax() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        const int tax = luxuryTaxAmount();
        if (player.isShieldActive()) {
            player.setShieldActive(false);
            controller_.addToast("Shield menahan pajak.", playerAccent(controller_.currentBackendPlayerIndex()));
        } else if (player.getMoney() < player.effectiveCost(tax)) {
            controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, tax, true);
            controller_.addToast("Dana kurang untuk membayar pajak.", RED);
        } else {
            controller_.backendGame_.getTaxManager().processTaxPayment(player, tax);
        }
        controller_.addLog(player.getUsername(), "PAJAK", "Membayar PBM M" + std::to_string(tax) + ".");
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
    }
}

void GUITileController::activateFestivalOnSelectedTile() {
    const std::vector<int> options = currentPlayerStreetOptions();
    if (options.empty()) {
        controller_.addToast("Belum ada street untuk festival.", RED);
        return;
    }

    const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    StreetTile* street = controller_.streetFromUi(options.at(selected));
    if (street == nullptr) {
        controller_.addToast("Street tidak valid.", RED);
        return;
    }

    try {
        controller_.backendGame_.getFestivalManager().activateFestival(*street);
        controller_.addLog(controller_.backendGame_.getCurrentPlayer().getUsername(), "FESTIVAL", "Mengaktifkan festival di " + street->getName() + ".");
        controller_.addToast("Festival aktif di " + street->getName() + ".", groupAccent(street->getColorGroup()));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::payJailFine() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        const int fine = jailFineAmount();
        if (player.getMoney() < player.effectiveCost(fine)) {
            controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, fine, true);
            controller_.syncViewFromBackend();
            openLiquidation();
            return;
        }

        controller_.backendGame_.getJailManager().payJailFine(player, fine);
        controller_.backendGame_.getJailManager().releaseFromJail(player);
        controller_.addLog(player.getUsername(), "JAIL", "Membayar denda jail M" + std::to_string(fine) + ".");
        controller_.addToast("Denda jail dibayar.", SKYBLUE);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::buildOnSelectedTile() {
    const std::vector<int> options = currentPlayerBuildOptions();
    if (options.empty()) {
        controller_.addToast("Tidak ada properti yang bisa dibangun.", RED);
        return;
    }

    const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    StreetTile* street = controller_.streetFromUi(options.at(selected));
    if (street == nullptr) {
        controller_.addToast("Street tidak valid.", RED);
        return;
    }

    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        const int before = street->getBuildingLevel();
        controller_.backendGame_.getPropertyManager().buildOnStreet(controller_.backendGame_.getBoard(), player, *street);
        if (street->getBuildingLevel() == before) {
            controller_.addToast("Tidak bisa membangun di properti ini.", RED);
            return;
        }
        controller_.addLog(player.getUsername(), "BANGUN", "Membangun di " + street->getName() + ".");
        controller_.addToast("Bangunan ditambahkan.", groupAccent(street->getColorGroup()));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::mortgageSelectedTile() {
    const std::vector<int> options = currentPlayerMortgageOptions();
    if (options.empty()) {
        controller_.addToast("Tidak ada aset yang bisa digadai.", RED);
        return;
    }

    const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    OwnableTile* property = controller_.ownableFromUi(options.at(selected));
    if (property == nullptr) {
        controller_.addToast("Properti tidak valid.", RED);
        return;
    }

    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        controller_.backendGame_.getPropertyManager().mortgageProperty(player, *property);
        controller_.addLog(player.getUsername(), "GADAI", "Menggadaikan " + property->getName() + ".");
        controller_.addToast("Aset digadai.", kindAccent(controller_.toGuiTileKind(*property), ""));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::redeemSelectedTile() {
    const std::vector<int> options = currentPlayerRedeemOptions();
    if (options.empty()) {
        controller_.addToast("Tidak ada aset yang bisa ditebus.", RED);
        return;
    }

    const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    OwnableTile* property = controller_.ownableFromUi(options.at(selected));
    if (property == nullptr) {
        controller_.addToast("Properti tidak valid.", RED);
        return;
    }

    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        if (player.getMoney() < player.effectiveCost(controller_.backendGame_.getPropertyManager().getRedeemCost(*property))) {
            controller_.addToast("Uang tidak cukup untuk menebus.", RED);
            return;
        }
        controller_.backendGame_.getPropertyManager().redeemProperty(player, *property);
        controller_.addLog(player.getUsername(), "TEBUS", "Menebus " + property->getName() + ".");
        controller_.addToast("Aset berhasil ditebus.", kindAccent(controller_.toGuiTileKind(*property), ""));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::liquidateSelectedTile() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        std::vector<OwnableTile*> properties = controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), player);
        if (properties.empty()) {
            controller_.addToast("Tidak ada aset untuk dijual.", RED);
            return;
        }

        const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(properties.size()) - 1));
        OwnableTile* property = properties.at(selected);
        const int saleValue = controller_.backendGame_.getPropertyManager().calculateSellToBankValue(*property);
        controller_.backendGame_.getPropertyManager().sellPropertyToBank(player, *property);
        property->setOwnershipStatus(OwnershipStatus::BANK);
        if (controller_.backendGame_.getBankruptcyManager().isBankruptcyActive()) {
            controller_.backendGame_.getBankruptcyManager().resolveLiquidationAction(player, "SELL");
        }
        controller_.addLog(player.getUsername(), "LIKUIDASI", "Menjual " + property->getName() + " ke bank.");
        controller_.addToast("Aset dijual seharga M" + std::to_string(saleValue) + ".", kindAccent(controller_.toGuiTileKind(*property), ""));
        controller_.syncViewFromBackend();
        if (!controller_.backendGame_.getBankruptcyManager().isBankruptcyActive()) {
            controller_.closeOverlay();
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::declareBankrupt() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        std::vector<OwnableTile*> properties = controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), player);
        for (OwnableTile* property : properties) {
            if (property == nullptr) {
                continue;
            }
            if (StreetTile* street = dynamic_cast<StreetTile*>(property)) {
                street->sellBuildings();
            }
            property->setOwner(nullptr);
            property->setOwnershipStatus(OwnershipStatus::BANK);
            property->setFestivalState(1, 0);
        }
        controller_.discardAllCards(player);
        controller_.backendGame_.getJailManager().releaseFromJail(player);
        controller_.backendGame_.getBankruptcyManager().declareBankrupt(player, nullptr);
        controller_.addLog(player.getUsername(), "BANGKRUT", "Pemain keluar dari permainan.");
        controller_.addToast(player.getUsername() + " bangkrut.", RED);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        if (controller_.backendGame_.isGameOver()) {
            controller_.openGameOver();
        } else {
            controller_.endTurn();
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

std::vector<int> GUITileController::currentPlayerStreetOptions() const {
    std::vector<int> streets;
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return streets;
    }

    for (OwnableTile* property : controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), controller_.backendGame_.getPlayers().at(playerIndex))) {
        if (dynamic_cast<StreetTile*>(property) != nullptr) {
            streets.push_back(controller_.uiTileIndexFromBackend(property->getIndex()));
        }
    }
    return streets;
}

std::vector<int> GUITileController::currentPlayerBuildOptions() const {
    std::vector<int> buildable;
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return buildable;
    }

    Player& player = const_cast<Player&>(controller_.backendGame_.getPlayers().at(playerIndex));
    for (OwnableTile* property : controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), player)) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr && controller_.backendGame_.getPropertyManager().canBuild(controller_.backendGame_.getBoard(), player, *street)) {
            buildable.push_back(controller_.uiTileIndexFromBackend(street->getIndex()));
        }
    }
    return buildable;
}

std::vector<int> GUITileController::currentPlayerMortgageOptions() const {
    std::vector<int> options;
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return options;
    }

    for (OwnableTile* property : controller_.backendGame_.getPropertyManager().getMortgageableProperties(controller_.backendGame_, controller_.backendGame_.getPlayers().at(playerIndex))) {
        if (property != nullptr) {
            options.push_back(controller_.uiTileIndexFromBackend(property->getIndex()));
        }
    }
    return options;
}

std::vector<int> GUITileController::currentPlayerRedeemOptions() const {
    std::vector<int> options;
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return options;
    }

    for (OwnableTile* property : controller_.backendGame_.getPropertyManager().getRedeemableProperties(controller_.backendGame_, controller_.backendGame_.getPlayers().at(playerIndex))) {
        if (property != nullptr) {
            options.push_back(controller_.uiTileIndexFromBackend(property->getIndex()));
        }
    }
    return options;
}

int GUITileController::moveBackendPlayer(Player& player, int steps) {
    const int boardSize = controller_.backendGame_.getBoard().getBoardSize();
    if (boardSize <= 0) {
        return player.getPosition();
    }

    const int oldPosition = controller_.normalizedBackendTileIndex(player.getPosition());
    const int zeroBased = oldPosition - 1;
    const int destinationZero = ((zeroBased + steps) % boardSize + boardSize) % boardSize;
    const int newPosition = destinationZero + 1;

    if (steps > 0 && destinationZero < zeroBased) {
        int salary = controller_.backendGame_.getConfigManager().getGoSalary();
        std::shared_ptr<Tile> goTile = controller_.backendGame_.getBoard().getTile(1);
        GoTile* go = dynamic_cast<GoTile*>(goTile.get());
        if (go != nullptr) {
            salary = go->getSalary();
        }
        player += salary;
        controller_.addLog(player.getUsername(), "GO", "Melewati GO dan menerima M" + std::to_string(salary) + ".");
        controller_.addToast("Melewati GO: +" + std::to_string(salary) + ".", tileToolkit.theme().getGold());
    }

    player.moveTo(newPosition);
    return newPosition;
}

void GUITileController::resolveBackendLanding(int backendTileIndex, bool fromMovement) {
    if (!controller_.backendGame_.isGameRunning() || controller_.backendGame_.getPlayers().empty()) {
        return;
    }

    const int normalizedIndex = controller_.normalizedBackendTileIndex(backendTileIndex);
    controller_.appState_.getGame().setSelectedTile(controller_.uiTileIndexFromBackend(normalizedIndex));
    Tile* tile = controller_.backendGame_.getBoard().getTile(normalizedIndex).get();
    if (tile == nullptr) {
        controller_.syncViewFromBackend();
        return;
    }

    Player& player = controller_.backendGame_.getCurrentPlayer();
    OverlayType overlayToOpen = OverlayType::None;
    int overlayTile = controller_.uiTileIndexFromBackend(normalizedIndex);

    try {
        switch (tile->onLand()) {
            case Tile::TileType::Street: {
                StreetTile& street = static_cast<StreetTile&>(*tile);
                Player* owner = street.getOwner();
                if (owner == nullptr || street.getOwnershipStatus() == OwnershipStatus::BANK) {
                    overlayToOpen = OverlayType::Purchase;
                    break;
                }
                if (owner == &player) {
                    controller_.addToast("Mendarat di properti sendiri.", groupAccent(street.getColorGroup()));
                    break;
                }
                if (street.isMortgaged()) {
                    controller_.addToast("Properti tergadai, tidak menarik sewa.", groupAccent(street.getColorGroup()));
                    break;
                }
                RentContext rentContext = controller_.backendGame_.getPropertyManager().createRentContext(
                    controller_.backendGame_.getBoard(),
                    controller_.backendGame_.getConfigManager(),
                    controller_.backendGame_.getDice(),
                    street
                );
                const int rent = street.calculateRent(rentContext);
                if (player.isShieldActive()) {
                    player.setShieldActive(false);
                    controller_.addToast("Shield menahan sewa.", playerAccent(controller_.currentBackendPlayerIndex()));
                    break;
                }
                if (player.getMoney() < player.effectiveCost(rent)) {
                    controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, owner, rent, false);
                    controller_.addLog(player.getUsername(), "SEWA", "Dana kurang untuk membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                    controller_.addToast("Dana kurang untuk membayar rent.", RED);
                    break;
                }
                player -= rent;
                *owner += player.effectiveCost(rent);
                controller_.addLog(player.getUsername(), "SEWA", "Membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                controller_.addToast("Bayar rent " + std::to_string(rent) + ".", groupAccent(street.getColorGroup()));
                break;
            }
            case Tile::TileType::Railroad:
            case Tile::TileType::Utility: {
                OwnableTile& property = static_cast<OwnableTile&>(*tile);
                Player* owner = property.getOwner();
                if (owner == nullptr || property.getOwnershipStatus() == OwnershipStatus::BANK) {
                    property.setOwner(&player);
                    property.setOwnershipStatus(OwnershipStatus::OWNED);
                    controller_.addLog(player.getUsername(), "AKUISISI", property.getName() + " menjadi milik pemain.");
                    controller_.addToast(property.getName() + " menjadi milikmu.", kindAccent(controller_.toGuiTileKind(property), ""));
                    break;
                }
                if (owner == &player) {
                    controller_.addToast("Mendarat di properti sendiri.", kindAccent(controller_.toGuiTileKind(property), ""));
                    break;
                }
                if (property.isMortgaged()) {
                    controller_.addToast("Properti tergadai, tidak menarik sewa.", kindAccent(controller_.toGuiTileKind(property), ""));
                    break;
                }
                RentContext rentContext = controller_.backendGame_.getPropertyManager().createRentContext(
                    controller_.backendGame_.getBoard(),
                    controller_.backendGame_.getConfigManager(),
                    controller_.backendGame_.getDice(),
                    property
                );
                const int rent = property.calculateRent(rentContext);
                if (player.isShieldActive()) {
                    player.setShieldActive(false);
                    controller_.addToast("Shield menahan sewa.", playerAccent(controller_.currentBackendPlayerIndex()));
                    break;
                }
                if (player.getMoney() < player.effectiveCost(rent)) {
                    controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, owner, rent, false);
                    controller_.addLog(player.getUsername(), "SEWA", "Dana kurang untuk membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                    controller_.addToast("Dana kurang untuk membayar rent.", RED);
                    break;
                }
                player -= rent;
                *owner += player.effectiveCost(rent);
                controller_.addLog(player.getUsername(), "SEWA", "Membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                controller_.addToast("Bayar rent " + std::to_string(rent) + ".", kindAccent(controller_.toGuiTileKind(property), ""));
                break;
            }
            case Tile::TileType::Chance:
                controller_.syncViewFromBackend();
                controller_.openRandomCardDraw(kChanceDeckKey);
                return;
            case Tile::TileType::CommunityChest:
                controller_.syncViewFromBackend();
                controller_.openRandomCardDraw(kCommunityDeckKey);
                return;
            case Tile::TileType::IncomeTax:
                overlayToOpen = OverlayType::IncomeTax;
                break;
            case Tile::TileType::LuxuryTax:
                overlayToOpen = OverlayType::LuxuryTax;
                break;
            case Tile::TileType::Festival:
                overlayToOpen = OverlayType::Festival;
                break;
            case Tile::TileType::GoToJail:
                if (player.isShieldActive()) {
                    player.setShieldActive(false);
                    controller_.addToast("Shield menahan efek jail.", playerAccent(controller_.currentBackendPlayerIndex()));
                    break;
                }
                controller_.backendGame_.getJailManager().sendToJail(player);
                player.moveTo(controller_.backendTileIndexFromUi(findJailIndex()));
                controller_.addLog(player.getUsername(), "JAIL", fromMovement ? "Efek kartu/perpindahan." : "Mendarat di Go To Jail.");
                overlayToOpen = OverlayType::Jail;
                overlayTile = findJailIndex();
                break;
            case Tile::TileType::Go:
                controller_.addToast("Mendarat di GO.", tileToolkit.theme().getGold());
                break;
            case Tile::TileType::Jail:
                controller_.addToast(player.isJailed() ? "Masih berada di jail." : "Hanya berkunjung ke jail.", tileToolkit.theme().getDanger());
                break;
            case Tile::TileType::FreeParking:
                controller_.addToast("Istirahat sejenak di Free Parking.", tileToolkit.theme().getPaper());
                break;
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
    }

    controller_.syncViewFromBackend();
    if (overlayToOpen == OverlayType::Purchase) {
        openPurchase(overlayTile);
    } else if (overlayToOpen == OverlayType::IncomeTax) {
        openIncomeTax();
    } else if (overlayToOpen == OverlayType::LuxuryTax) {
        openLuxuryTax();
    } else if (overlayToOpen == OverlayType::Festival) {
        openFestival();
    } else if (overlayToOpen == OverlayType::Jail) {
        openJail();
    } else {
        controller_.maybeOpenLiquidation();
    }
}
