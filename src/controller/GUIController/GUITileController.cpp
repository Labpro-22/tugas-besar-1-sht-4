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

Color GUITileController::playerAccent(int index) {
    UiToolkit tileToolkit;
    switch (index) {
        case 0: return tileToolkit.theme().getCoral();
        case 1: return tileToolkit.theme().getTeal();
        case 2: return tileToolkit.theme().getGold();
        default: return tileToolkit.theme().getNavy();
    }
}

std::string GUITileController::normalizeKey(std::string value) {
    std::string normalized;
    for (char ch : value) {
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
    }
    return normalized;
}

std::string GUITileController::upperCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

Color GUITileController::groupAccent(const std::string& group) {
    UiToolkit tileToolkit;
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

std::string GUITileController::colorDisplayName(const std::string& group) {
    const std::string key = normalizeKey(group);
    if (key == "COKLAT" || key == "BROWN") return "COKLAT";
    if (key == "BIRUMUDA" || key == "LIGHTBLUE") return "BIRU MUDA";
    if (key == "PINK") return "PINK";
    if (key == "ORANGE" || key == "ORANYE") return "ORANGE";
    if (key == "MERAH" || key == "RED") return "MERAH";
    if (key == "KUNING" || key == "YELLOW") return "KUNING";
    if (key == "HIJAU" || key == "GREEN") return "HIJAU";
    if (key == "BIRUTUA" || key == "NAVY" || key == "DARKBLUE") return "BIRU TUA";
    if (key == "STASIUN" || key == "RAILROAD") return "STASIUN";
    if (key == "UTILITAS" || key == "UTILITY" || key == "AB") return "UTILITAS";
    return "AKSI";
}

std::string GUITileController::ownershipStatusText(OwnershipStatus status) {
    if (status == OwnershipStatus::BANK) return "BANK";
    if (status == OwnershipStatus::OWNED) return "OWNED";
    return "MORTGAGED";
}

std::string GUITileController::deedOwnershipStatusText(const OwnableTile& tile) {
    if (tile.isMortgaged()) return ownershipStatusText(OwnershipStatus::MORTGAGED);
    return ownershipStatusText(tile.getOwnershipStatus());
}

std::string GUITileController::buildingText(const StreetTile& tile) {
    if (tile.hasHotel()) {
        return "Hotel";
    }
    if (tile.getBuildingLevel() <= 0) {
        return "Tanpa bangunan";
    }
    return std::to_string(tile.getBuildingLevel()) + " rumah";
}

Color GUITileController::kindAccent(TileKind kind, const std::string& group) {
    UiToolkit tileToolkit;
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

GUITileController::GUITileController(GUIControllerContext& controller)
    : controller_(controller) {
    controller_.openJailCallback = [this]() { openJail(); };
    controller_.moveBackendPlayerCallback = [this](Player& player, int steps) { return moveBackendPlayer(player, steps); };
    controller_.resolveBackendLandingCallback = [this](int backendTileIndex, bool fromMovement) {
        resolveBackendLanding(backendTileIndex, fromMovement);
    };
}

Player* GUITileController::activeLiquidationPlayer() const {
    const int playerIndex = activeLiquidationPlayerIndex();
    std::vector<Player>& players = controller_.backendGame_.getPlayers();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return nullptr;
    }
    return &players.at(playerIndex);
}

int GUITileController::activeLiquidationPlayerIndex() const {
    Player* debtor = controller_.backendGame_.getBankruptcyManager().getPendingDebtor();
    if (debtor != nullptr) {
        const std::vector<Player>& players = controller_.backendGame_.getPlayers();
        for (int index = 0; index < static_cast<int>(players.size()); index++) {
            if (&players.at(index) == debtor) {
                return index;
            }
        }
    }
    return controller_.currentBackendPlayerIndex();
}

bool GUITileController::isCurrentTurnPlayer(const Player& player) const {
    try {
        return &controller_.backendGame_.getCurrentPlayer() == &player;
    } catch (...) {
        return false;
    }
}

size_t GUITileController::activePlayerCount() const {
    size_t count = 0;
    for (const Player& player : controller_.backendGame_.getPlayers()) {
        if (!player.isBankrupt()) {
            count++;
        }
    }
    return count;
}

bool GUITileController::colorGroupHasBuildings(const StreetTile& tile) const {
    for (const std::shared_ptr<StreetTile>& street : controller_.backendGame_.getBoard().getStreetTileByColorGroup(tile.getColorGroup())) {
        if (street != nullptr && street->getBuildingLevel() > 0) {
            return true;
        }
    }
    return false;
}

int GUITileController::sellColorGroupBuildings(Player& player, const StreetTile& tile) const {
    int total = 0;
    for (const std::shared_ptr<StreetTile>& street : controller_.backendGame_.getBoard().getStreetTileByColorGroup(tile.getColorGroup())) {
        if (street == nullptr || street->getOwner() != &player || street->getBuildingLevel() <= 0) {
            continue;
        }
        const int saleValue = street->getBuildingValue() / 2;
        total += saleValue;
        player += saleValue;
        controller_.addLog(player.getUsername(), "JUAL_BGN", "Menjual bangunan di " + street->getName() + " seharga M" + std::to_string(saleValue) + ".");
        street->sellBuildings();
    }
    return total;
}

void GUITileController::returnPropertyToBank(OwnableTile& property) const {
    if (StreetTile* street = dynamic_cast<StreetTile*>(&property)) {
        street->sellBuildings();
    }
    property.setOwner(nullptr);
    property.setOwnershipStatus(OwnershipStatus::BANK);
    property.setFestivalState(1, 0);
}

int GUITileController::getMortgageValue(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    return ownable != nullptr ? ownable->getMortgageValue() : tile.getMortgageValue();
}

int GUITileController::getRedeemCost(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    if (ownable == nullptr) {
        return getMortgageValue(tile);
    }

    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return controller_.backendGame_.getPropertyManager().getRedeemCost(*ownable);
    }

    const Player& player = controller_.backendGame_.getPlayers().at(playerIndex);
    return player.effectiveCost(controller_.backendGame_.getPropertyManager().getRedeemCost(*ownable));
}

int GUITileController::getSellToBankValue(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    return ownable != nullptr ? controller_.backendGame_.getPropertyManager().calculateSellToBankValue(*ownable) : 0;
}

TileDeedInfo GUITileController::deedInfoForTile(const TileInfo& tile) const {
    const OwnableTile* ownable = controller_.ownableFromUi(tile.getIndex());
    if (ownable == nullptr) {
        return {};
    }

    std::string title = "[PROPERTI] " + upperCopy(ownable->getName()) + " (" + ownable->getCode() + ")";
    std::vector<std::string> moneyRowLabels;
    std::vector<int> moneyRowValues;
    std::vector<std::string> detailRowLabels;
    std::vector<std::string> detailRowValues;

    if (const StreetTile* street = dynamic_cast<const StreetTile*>(ownable)) {
        title = "[" + colorDisplayName(street->getColorGroup()) + "] " +
                upperCopy(ownable->getName()) + " (" + ownable->getCode() + ")";
        const std::vector<std::string> labels = {
            "Sewa (unimproved)",
            "Sewa (1 rumah)",
            "Sewa (2 rumah)",
            "Sewa (3 rumah)",
            "Sewa (4 rumah)",
            "Sewa (hotel)"
        };
        const std::vector<int>& rents = street->getRentLevels();
        for (size_t index = 0; index < labels.size() && index < rents.size(); index++) {
            moneyRowLabels.push_back(labels.at(index));
            moneyRowValues.push_back(rents.at(index));
        }
        moneyRowLabels.push_back("Harga Rumah");
        moneyRowValues.push_back(street->getHouseBuildCost());
        moneyRowLabels.push_back("Harga Hotel");
        moneyRowValues.push_back(street->getHotelBuildCost());

        detailRowLabels.push_back("Bangunan");
        detailRowValues.push_back(buildingText(*street));
        if (street->getFestivalDuration() > 0 && street->getFestivalMultiplier() > 1) {
            detailRowLabels.push_back("Festival");
            detailRowValues.push_back(
                "x" + std::to_string(street->getFestivalMultiplier()) +
                ", " + std::to_string(street->getFestivalDuration()) + " giliran"
            );
        }
    } else if (dynamic_cast<const RailroadTile*>(ownable) != nullptr) {
        title = "[STASIUN] " + upperCopy(ownable->getName()) + " (" + ownable->getCode() + ")";
        detailRowLabels.push_back("Sewa");
        detailRowValues.push_back("Jumlah stasiun");
    } else if (dynamic_cast<const UtilityTile*>(ownable) != nullptr) {
        title = "[UTILITAS] " + upperCopy(ownable->getName()) + " (" + ownable->getCode() + ")";
        detailRowLabels.push_back("Sewa");
        detailRowValues.push_back("Dadu x util");
    }

    int purchasePrice = ownable->getPurchasePrice();
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex >= 0 && playerIndex < static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        const Player& player = controller_.backendGame_.getPlayers().at(playerIndex);
        purchasePrice = player.effectiveCost(purchasePrice);
        for (int& value : moneyRowValues) {
            value = player.effectiveCost(value);
        }
    }

    const Player* owner = ownable->getOwner();
    const std::string ownerName = owner != nullptr ? owner->getUsername() : "BANK";
    return TileDeedInfo(
        true,
        title,
        purchasePrice,
        ownable->getMortgageValue(),
        moneyRowLabels,
        moneyRowValues,
        detailRowLabels,
        detailRowValues,
        deedOwnershipStatusText(*ownable),
        ownerName
    );
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
    return total;
}

int GUITileController::jailFineAmount() const {
    return controller_.backendGame_.getConfigManager().getJailFine();
}

int GUITileController::gridCellCount() const {
    const int boardSize = static_cast<int>(controller_.appState_.getGame().getBoard().size());
    if (boardSize < 4) {
        return kGridCells;
    }
    return (boardSize + 7) / 4;
}

Rectangle GUITileController::boardTileRect(Rectangle square, int index) const {
    const int gridCells = gridCellCount();
    const int edge = gridCells - 1;
    const int perimeter = edge * 4;
    const int normalized = perimeter > 0 ? ((index % perimeter) + perimeter) % perimeter : index;
    const float cell = square.width / static_cast<float>(gridCells);

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

    if (controller_.ownableFromUi(tileIndex) == nullptr) {
        controller_.addToast("Petak ini tidak bisa dilelang.", RED);
        return;
    }

    const std::vector<Player>& players = controller_.backendGame_.getPlayers();
    std::vector<bool> passed(players.size(), false);
    int activeCount = 0;
    for (int index = 0; index < static_cast<int>(players.size()); index++) {
        if (players.at(index).isBankrupt()) {
            passed.at(index) = true;
        } else {
            activeCount++;
        }
    }
    if (activeCount <= 0) {
        controller_.addToast("Tidak ada peserta lelang aktif.", RED);
        return;
    }

    const int triggerIndex = controller_.currentBackendPlayerIndex();
    int firstBidder = triggerIndex;
    for (int offset = 1; offset <= static_cast<int>(players.size()); offset++) {
        const int candidate = (triggerIndex + offset) % static_cast<int>(players.size());
        if (!passed.at(candidate)) {
            firstBidder = candidate;
            break;
        }
    }

    controller_.appState_.getOverlay().setAuction({
        tileIndex,
        firstBidder,
        0,
        -1,
        passed,
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
    if (!controller_.guiTurnStarted_ || controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Bangun hanya bisa dilakukan sebelum lempar dadu.", RED);
        return;
    }
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Build);
}

void GUITileController::openMortgage() {
    if (!controller_.guiTurnStarted_ || controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Gadai hanya bisa dilakukan sebelum lempar dadu.", RED);
        return;
    }
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Mortgage);
}

void GUITileController::openRedeem() {
    if (!controller_.guiTurnStarted_ || controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Tebus hanya bisa dilakukan sebelum lempar dadu.", RED);
        return;
    }
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.openOverlay(OverlayType::Redeem);
}

void GUITileController::openLiquidation() {
    controller_.appState_.getOverlay().setSelectedIndex(0);
    controller_.appState_.getOverlay().setSelectedPlayer(activeLiquidationPlayerIndex());
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

bool GUITileController::canActiveAuctionBidderPass() const {
    const AuctionState& auction = controller_.appState_.getOverlay().getAuction();
    if (auction.getHighestBidder() >= 0) {
        return true;
    }

    const int remaining = static_cast<int>(std::count(auction.getPassed().begin(), auction.getPassed().end(), false));
    return remaining > 1;
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
        controller_.finishTurnAfterDiceIfReady();
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

void GUITileController::auctionPlaceBid(int targetBid) {
    OverlayState& overlay = controller_.appState_.getOverlay();
    AuctionState auction = overlay.getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return;
    }
    if (auction.getPassed().at(bidderIndex) || controller_.backendGame_.getPlayers().at(bidderIndex).isBankrupt()) {
        controller_.addToast("Bidder ini sudah tidak aktif di lelang.", RED);
        return;
    }

    const bool hasBidder = auction.getHighestBidder() >= 0;
    const int minimumBid = hasBidder ? auction.getHighestBid() + 1 : 0;
    if (targetBid < minimumBid) {
        controller_.addToast("Bid harus minimal M" + std::to_string(minimumBid) + ".", RED);
        return;
    }

    Player& bidder = controller_.backendGame_.getPlayers().at(bidderIndex);
    if (bidder.getMoney() < bidder.effectiveCost(targetBid)) {
        controller_.addToast("Bidder tidak punya uang cukup.", RED);
        return;
    }

    controller_.backendGame_.getAuctionManager().placeBid(bidder, targetBid);
    auction.setHighestBid(targetBid);
    auction.setHighestBidder(bidderIndex);

    std::vector<bool> passed(controller_.backendGame_.getPlayers().size(), false);
    for (int index = 0; index < static_cast<int>(controller_.backendGame_.getPlayers().size()); index++) {
        if (controller_.backendGame_.getPlayers().at(index).isBankrupt()) {
            passed.at(index) = true;
        }
    }
    auction.setPassed(passed);

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
    controller_.addLog(bidder.getUsername(), "BID", "Menawar M" + std::to_string(bidder.effectiveCost(targetBid)) + ".");
    controller_.syncViewFromBackend();
    finalizeAuction();
}

void GUITileController::auctionPass() {
    OverlayState& overlay = controller_.appState_.getOverlay();
    AuctionState auction = overlay.getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return;
    }
    if (!canActiveAuctionBidderPass()) {
        auction.setBidError("Pemain terakhir wajib melakukan bid.");
        overlay.setAuction(auction);
        controller_.addToast("Pemain terakhir wajib bid.", RED);
        return;
    }

    std::vector<bool> passed = auction.getPassed();
    if (passed.at(bidderIndex)) {
        return;
    }
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
    controller_.syncViewFromBackend();
    finalizeAuction();
}

void GUITileController::finalizeAuction() {
    OverlayState& overlay = controller_.appState_.getOverlay();
    AuctionState auction = overlay.getAuction();
    const int remaining = static_cast<int>(std::count(auction.getPassed().begin(), auction.getPassed().end(), false));
    if ((auction.getHighestBidder() < 0 && remaining > 0) ||
        (auction.getHighestBidder() >= 0 && remaining > 1)) {
        return;
    }

    OwnableTile* ownable = controller_.ownableFromUi(auction.getTileIndex());
    if (ownable != nullptr && auction.getHighestBidder() >= 0) {
        const int winnerIndex = auction.getHighestBidder();
        if (winnerIndex >= 0 && winnerIndex < static_cast<int>(controller_.backendGame_.getPlayers().size())) {
            Player& winner = controller_.backendGame_.getPlayers().at(winnerIndex);
            const int finalPayment = winner.effectiveCost(auction.getHighestBid());
            winner -= auction.getHighestBid();
            ownable->setOwner(&winner);
            ownable->setOwnershipStatus(OwnershipStatus::OWNED);
            controller_.addLog(winner.getUsername(), "MENANG_LELANG", "Menang " + ownable->getName() + " seharga M" + std::to_string(finalPayment) + ".");
            controller_.addToast("Lelang dimenangkan " + winner.getUsername() + ".", kindAccent(controller_.toGuiTileKind(*ownable), ""));
        }
    } else if (ownable != nullptr) {
        ownable->setOwner(nullptr);
        ownable->setOwnershipStatus(OwnershipStatus::BANK);
        controller_.addLog("BANK", "LELANG", "Tidak ada pemenang untuk " + ownable->getName() + ".");
        controller_.addToast("Tidak ada pemenang lelang.", RED);
    }

    controller_.closeOverlay();
    controller_.syncViewFromBackend();
    controller_.finishTurnAfterDiceIfReady();
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
        const int actualTax = player.effectiveCost(tax);
        if (player.isShieldActive()) {
            player.setShieldActive(false);
            controller_.addLog(player.getUsername(), "PAJAK", "Shield menahan PPh.");
            controller_.addToast("Shield menahan pajak.", playerAccent(controller_.currentBackendPlayerIndex()));
        } else if (player.getMoney() < actualTax) {
            controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, tax, true);
            controller_.addLog(player.getUsername(), "PAJAK", "Dana kurang untuk membayar PPh M" + std::to_string(actualTax) + ".");
            controller_.addToast("Dana kurang untuk membayar pajak.", RED);
        } else {
            player -= tax;
            controller_.addLog(player.getUsername(), "PAJAK", "Membayar PPh M" + std::to_string(actualTax) + ".");
        }
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
        controller_.finishTurnAfterDiceIfReady();
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
        const int actualTax = player.effectiveCost(tax);
        if (player.isShieldActive()) {
            player.setShieldActive(false);
            controller_.addLog(player.getUsername(), "PAJAK", "Shield menahan PBM.");
            controller_.addToast("Shield menahan pajak.", playerAccent(controller_.currentBackendPlayerIndex()));
        } else if (player.getMoney() < actualTax) {
            controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, tax, true);
            controller_.addLog(player.getUsername(), "PAJAK", "Dana kurang untuk membayar PBM M" + std::to_string(actualTax) + ".");
            controller_.addToast("Dana kurang untuk membayar pajak.", RED);
        } else {
            player -= tax;
            controller_.addLog(player.getUsername(), "PAJAK", "Membayar PBM M" + std::to_string(actualTax) + ".");
        }
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
        controller_.maybeOpenLiquidation();
    }
}

void GUITileController::activateFestivalOnSelectedTile() {
    const std::vector<int> options = currentPlayerOwnableOptions();
    if (options.empty()) {
        controller_.addToast("Belum ada properti untuk festival.", RED);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.finishTurnAfterDiceIfReady();
        return;
    }

    const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    OwnableTile* property = controller_.ownableFromUi(options.at(selected));
    if (property == nullptr) {
        controller_.addToast("Properti tidak valid.", RED);
        return;
    }

    try {
        const int oldMultiplier = property->getFestivalMultiplier();
        controller_.backendGame_.getFestivalManager().activateFestival(*property);
        const int newMultiplier = property->getFestivalMultiplier();

        std::string festivalAction = "Aktivasi";
        if (oldMultiplier > 1) festivalAction = "Penguatan";
        if (oldMultiplier >= 8) festivalAction = "Perpanjangan";

        controller_.addLog(
            controller_.backendGame_.getCurrentPlayer().getUsername(),
            "FESTIVAL",
            festivalAction + " festival di " + property->getName() + ", x" + std::to_string(oldMultiplier) + " -> x" + std::to_string(newMultiplier) + "."
        );

        const StreetTile* street = dynamic_cast<const StreetTile*>(property);
        const Color accent = street != nullptr ? groupAccent(street->getColorGroup()) : kindAccent(controller_.toGuiTileKind(*property), "");
        controller_.addToast("Festival aktif di " + property->getName() + ".", accent);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::payJailFine() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        const int fine = jailFineAmount();
        const int actualFine = player.effectiveCost(fine);
        if (player.getMoney() < player.effectiveCost(fine)) {
            controller_.backendGame_.getJailManager().releaseFromJail(player);
            controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, fine, true);
            controller_.addLog(player.getUsername(), "JAIL", "Dana kurang untuk denda jail - likuidasi.");
            controller_.addToast("Dana kurang - likuidasi aset.", RED);
            controller_.syncViewFromBackend();
            openLiquidation();
            return;
        }

        controller_.backendGame_.getJailManager().payJailFine(player, fine);
        controller_.backendGame_.getJailManager().releaseFromJail(player);
        controller_.addLog(player.getUsername(), "JAIL", "Membayar denda jail M" + std::to_string(actualFine) + ".");
        controller_.addToast("Denda jail dibayar.", SKYBLUE);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::attemptJailRoll() {
    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        if (!player.isJailed()) {
            controller_.closeOverlay();
            controller_.syncViewFromBackend();
            return;
        }
        if (player.getFailedJailRolls() >= 3) {
            controller_.addToast("Sudah gagal 3 kali. Wajib bayar denda jail.", RED);
            return;
        }

        const bool released = controller_.backendGame_.getJailManager().tryRollForRelease(controller_.backendGame_.getDice(), player);
        const int dieOne = controller_.backendGame_.getDice().getDie1();
        const int dieTwo = controller_.backendGame_.getDice().getDie2();
        const int total = controller_.backendGame_.getDice().getTotal();
        controller_.backendGame_.getTurnManager().setRolledThisTurn(true);
        controller_.diceRolledThisTurn_ = true;

        if (!released) {
            player.setFailedJailRolls(std::min(3, player.getFailedJailRolls() + 1));
            controller_.addLog(
                player.getUsername(),
                "JAIL",
                "Gagal keluar jail dengan dadu " + std::to_string(dieOne) + "+" + std::to_string(dieTwo) + "."
            );
            controller_.addToast("Belum double. Tetap di jail.", RED);
            controller_.closeOverlay();
            controller_.syncViewFromBackend();
            controller_.finishTurnAfterDiceIfReady();
            return;
        }

        const int destination = moveBackendPlayer(player, total);
        controller_.addLog(
            player.getUsername(),
            "JAIL",
            "Double " + std::to_string(dieOne) + "+" + std::to_string(dieTwo) + ", keluar jail."
        );
        controller_.addToast("Double! Keluar dari jail.", SKYBLUE);
        controller_.closeOverlay();
        resolveBackendLanding(destination, false);
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::buildOnSelectedTile() {
    if (controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Bangun hanya bisa dilakukan sebelum lempar dadu.", RED);
        return;
    }
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
        controller_.backendGame_.getTurnManager().registerAction();
        controller_.addLog(player.getUsername(), "BANGUN", "Membangun di " + street->getName() + ".");
        controller_.addToast("Bangunan ditambahkan.", groupAccent(street->getColorGroup()));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::mortgageSelectedTile() {
    if (controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Gadai hanya bisa dilakukan sebelum lempar dadu.", RED);
        return;
    }
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
        if (StreetTile* street = dynamic_cast<StreetTile*>(property)) {
            if (colorGroupHasBuildings(*street)) {
                const int buildingValue = sellColorGroupBuildings(player, *street);
                controller_.addToast("Bangunan satu warna dijual: M" + std::to_string(buildingValue) + ".", groupAccent(street->getColorGroup()));
            }
        }
        controller_.backendGame_.getPropertyManager().mortgageProperty(player, *property);
        controller_.backendGame_.getTurnManager().registerAction();
        controller_.addLog(player.getUsername(), "GADAI", "Menggadaikan " + property->getName() + ".");
        controller_.addToast("Aset digadai.", kindAccent(controller_.toGuiTileKind(*property), ""));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::redeemSelectedTile() {
    if (controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Tebus hanya bisa dilakukan sebelum lempar dadu.", RED);
        return;
    }
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
        const int redeemCost = player.effectiveCost(controller_.backendGame_.getPropertyManager().getRedeemCost(*property));
        controller_.backendGame_.getPropertyManager().redeemProperty(player, *property);
        controller_.backendGame_.getTurnManager().registerAction();
        controller_.addLog(player.getUsername(), "TEBUS", "Menebus " + property->getName() + " seharga M" + std::to_string(redeemCost) + ".");
        controller_.addToast("Aset berhasil ditebus.", kindAccent(controller_.toGuiTileKind(*property), ""));
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        controller_.finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::liquidateSelectedTile() {
    try {
        Player* liquidationPlayer = activeLiquidationPlayer();
        if (liquidationPlayer == nullptr) {
            controller_.addToast("Pemain likuidasi tidak valid.", RED);
            return;
        }
        Player& player = *liquidationPlayer;
        std::vector<OwnableTile*> properties = controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), player);
        if (properties.empty()) {
            controller_.addToast("Tidak ada aset untuk dijual.", RED);
            return;
        }

        const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(properties.size()) - 1));
        OwnableTile* property = properties.at(selected);
        const int saleValue = controller_.backendGame_.getPropertyManager().calculateSellToBankValue(*property);
        player += saleValue;
        returnPropertyToBank(*property);
        if (controller_.backendGame_.getBankruptcyManager().isBankruptcyActive() &&
            controller_.backendGame_.getBankruptcyManager().getPendingDebtor() == &player) {
            controller_.backendGame_.getBankruptcyManager().resolveLiquidationAction(player, "SELL");
        }
        controller_.addLog(player.getUsername(), "LIKUIDASI", "Menjual " + property->getName() + " ke bank.");
        controller_.addToast("Aset dijual seharga M" + std::to_string(saleValue) + ".", kindAccent(controller_.toGuiTileKind(*property), ""));
        controller_.syncViewFromBackend();
        if (controller_.backendGame_.getBankruptcyManager().isBankruptcyActive() || player.getMoney() < 0) {
            openLiquidation();
        } else {
            controller_.closeOverlay();
            if (isCurrentTurnPlayer(player)) {
                controller_.finishTurnAfterDiceIfReady();
            } else {
                controller_.finishTurnAfterDiceIfReady();
            }
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::mortgageLiquidationSelectedTile() {
    try {
        Player* liquidationPlayer = activeLiquidationPlayer();
        if (liquidationPlayer == nullptr) {
            controller_.addToast("Pemain likuidasi tidak valid.", RED);
            return;
        }
        Player& player = *liquidationPlayer;
        std::vector<OwnableTile*> properties = controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), player);
        std::vector<OwnableTile*> mortgageable;
        for (OwnableTile* property : properties) {
            if (property != nullptr &&
                controller_.backendGame_.getPropertyManager().canMortgage(player, *property)) {
                mortgageable.push_back(property);
            }
        }
        if (mortgageable.empty()) {
            controller_.addToast("Tidak ada aset yang bisa digadai.", RED);
            return;
        }

        const int selected = std::max(0, std::min(controller_.appState_.getOverlay().getSelectedIndex(), static_cast<int>(properties.size()) - 1));
        OwnableTile* property = properties.at(selected);
        if (std::find(mortgageable.begin(), mortgageable.end(), property) == mortgageable.end()) {
            controller_.addToast("Aset ini tidak bisa digadai.", RED);
            return;
        }

        if (StreetTile* street = dynamic_cast<StreetTile*>(property)) {
            if (colorGroupHasBuildings(*street)) {
                const int buildingValue = sellColorGroupBuildings(player, *street);
                controller_.addToast("Bangunan satu warna dijual: M" + std::to_string(buildingValue) + ".", groupAccent(street->getColorGroup()));
            }
        }

        const int mortgageValue = property->getMortgageValue();
        controller_.backendGame_.getPropertyManager().mortgageProperty(player, *property);
        if (controller_.backendGame_.getBankruptcyManager().isBankruptcyActive() &&
            controller_.backendGame_.getBankruptcyManager().getPendingDebtor() == &player) {
            controller_.backendGame_.getBankruptcyManager().resolveLiquidationAction(player, "MORTGAGE");
        }
        controller_.addLog(player.getUsername(), "LIKUIDASI", "Menggadaikan " + property->getName() + ".");
        controller_.addToast("Aset digadai seharga M" + std::to_string(mortgageValue) + ".", kindAccent(controller_.toGuiTileKind(*property), ""));
        controller_.syncViewFromBackend();
        if (controller_.backendGame_.getBankruptcyManager().isBankruptcyActive() || player.getMoney() < 0) {
            openLiquidation();
        } else {
            controller_.closeOverlay();
            controller_.finishTurnAfterDiceIfReady();
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUITileController::declareBankrupt() {
    try {
        Player* liquidationPlayer = activeLiquidationPlayer();
        if (liquidationPlayer == nullptr) {
            controller_.addToast("Pemain bangkrut tidak valid.", RED);
            return;
        }
        Player& player = *liquidationPlayer;
        std::vector<OwnableTile*> properties = controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), player);

        Player* creditor = controller_.backendGame_.getBankruptcyManager().getPendingCreditor();
        const bool debtToBank = controller_.backendGame_.getBankruptcyManager().isPendingDebtToBank();

        if (!debtToBank && creditor != nullptr) {
            const int remaining = std::max(0, player.getMoney());
            *creditor += remaining;
            player.setMoney(0);

            for (OwnableTile* property : properties) {
                if (property == nullptr) {
                    continue;
                }
                if (StreetTile* street = dynamic_cast<StreetTile*>(property)) {
                    street->sellBuildings();
                }
                property->setFestivalState(1, 0);
                property->setOwner(creditor);
                property->setOwnershipStatus(property->isMortgaged() ? OwnershipStatus::MORTGAGED : OwnershipStatus::OWNED);
                controller_.addLog(player.getUsername(), "ASET", property->getName() + " dialihkan ke " + creditor->getUsername() + ".");
            }
            controller_.addToast("Aset dialihkan ke " + creditor->getUsername() + ".", RED);
        } else {
            player.setMoney(0);
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
                controller_.addLog(player.getUsername(), "ASET", property->getName() + " dikembalikan ke Bank.");
            }
        }

        controller_.discardAllCards(player);
        controller_.backendGame_.getJailManager().releaseFromJail(player);
        controller_.backendGame_.getBankruptcyManager().declareBankrupt(player, creditor);
        controller_.backendGame_.getBankruptcyManager().clearSession();
        controller_.addLog(player.getUsername(), "BANGKRUT", "Pemain keluar dari permainan.");
        controller_.addToast(player.getUsername() + " bangkrut.", RED);
        controller_.closeOverlay();
        controller_.syncViewFromBackend();
        if (controller_.backendGame_.isGameOver()) {
            controller_.openGameOver();
        } else if (controller_.backendGame_.getBankruptcyManager().isBankruptcyActive()) {
            openLiquidation();
        } else {
            if (isCurrentTurnPlayer(player)) {
                controller_.endTurn();
            } else {
                controller_.finishTurnAfterDiceIfReady();
            }
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

std::vector<int> GUITileController::currentPlayerOwnableOptions() const {
    std::vector<int> options;
    const int playerIndex = controller_.currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(controller_.backendGame_.getPlayers().size())) {
        return options;
    }

    for (OwnableTile* property : controller_.backendGame_.getPropertyManager().getOwnedProperties(controller_.backendGame_.getBoard(), controller_.backendGame_.getPlayers().at(playerIndex))) {
        if (property != nullptr) {
            options.push_back(controller_.uiTileIndexFromBackend(property->getIndex()));
        }
    }
    return options;
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
    std::string fromCode = std::to_string(oldPosition);
    std::string toCode = std::to_string(newPosition);
    std::shared_ptr<Tile> fromTile = controller_.backendGame_.getBoard().getTile(oldPosition);
    std::shared_ptr<Tile> toTile = controller_.backendGame_.getBoard().getTile(newPosition);
    if (fromTile != nullptr) {
        fromCode = fromTile->getCode();
    }
    if (toTile != nullptr) {
        toCode = toTile->getCode();
    }
    controller_.addLog(player.getUsername(), "GERAK", "Bergerak " + std::to_string(steps) + " petak dari " + fromCode + " ke " + toCode + ".");
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
                const int actualRent = player.effectiveCost(rent);
                if (player.isShieldActive()) {
                    player.setShieldActive(false);
                    controller_.addLog(player.getUsername(), "SEWA", "Shield menahan sewa ke " + owner->getUsername() + ".");
                    controller_.addToast("Shield menahan sewa.", playerAccent(controller_.currentBackendPlayerIndex()));
                    break;
                }
                if (player.getMoney() < actualRent) {
                    controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, owner, rent, false);
                    controller_.addLog(player.getUsername(), "SEWA", "Dana kurang untuk membayar M" + std::to_string(actualRent) + " ke " + owner->getUsername() + ".");
                    controller_.addToast("Dana kurang untuk membayar rent.", RED);
                    break;
                }
                player -= rent;
                *owner += actualRent;
                controller_.addLog(player.getUsername(), "SEWA", "Membayar M" + std::to_string(actualRent) + " ke " + owner->getUsername() + ".");
                controller_.addToast("Bayar rent " + std::to_string(actualRent) + ".", groupAccent(street.getColorGroup()));
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
                const int actualRent = player.effectiveCost(rent);
                if (player.isShieldActive()) {
                    player.setShieldActive(false);
                    controller_.addLog(player.getUsername(), "SEWA", "Shield menahan sewa ke " + owner->getUsername() + ".");
                    controller_.addToast("Shield menahan sewa.", playerAccent(controller_.currentBackendPlayerIndex()));
                    break;
                }
                if (player.getMoney() < actualRent) {
                    controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, owner, rent, false);
                    controller_.addLog(player.getUsername(), "SEWA", "Dana kurang untuk membayar M" + std::to_string(actualRent) + " ke " + owner->getUsername() + ".");
                    controller_.addToast("Dana kurang untuk membayar rent.", RED);
                    break;
                }
                player -= rent;
                *owner += actualRent;
                controller_.addLog(player.getUsername(), "SEWA", "Membayar M" + std::to_string(actualRent) + " ke " + owner->getUsername() + ".");
                controller_.addToast("Bayar rent " + std::to_string(actualRent) + ".", kindAccent(controller_.toGuiTileKind(property), ""));
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
                {
                    const int previousPosition = player.getPosition();
                    std::string fromCode = std::to_string(previousPosition);
                    std::shared_ptr<Tile> previousTile = controller_.backendGame_.getBoard().getTile(previousPosition);
                    if (previousTile != nullptr) {
                        fromCode = previousTile->getCode();
                    }
                    controller_.backendGame_.getJailManager().sendToJail(player);
                    player.moveTo(controller_.backendTileIndexFromUi(findJailIndex()));
                    std::string toCode = std::to_string(player.getPosition());
                    std::shared_ptr<Tile> jailTile = controller_.backendGame_.getBoard().getTile(player.getPosition());
                    if (jailTile != nullptr) {
                        toCode = jailTile->getCode();
                    }
                    controller_.addLog(player.getUsername(), "GERAK", "Petak Pergi ke Penjara memindahkan bidak dari " + fromCode + " ke " + toCode + ".");
                }
                controller_.addLog(player.getUsername(), "JAIL", fromMovement ? "Efek kartu/perpindahan." : "Mendarat di Go To Jail.");
                controller_.addToast("Masuk penjara.", tileToolkit.theme().getDanger());
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
        controller_.finishTurnAfterDiceIfReady();
    }
}
