#include "controller/GUIController/GUIGameController.hpp"

#include "model/Dice.hpp"
#include "model/NimonException.hpp"
#include "model/cards/BirthdayCard.hpp"
#include "model/cards/CampaignFeeCard.hpp"
#include "model/cards/Card.hpp"
#include "model/cards/ChanceCard.hpp"
#include "model/cards/CommunityChestCard.hpp"
#include "model/cards/DemolitionCard.hpp"
#include "model/cards/DiscountCard.hpp"
#include "model/cards/DoctorFeeCard.hpp"
#include "model/cards/GoToJailCard.hpp"
#include "model/cards/GoToNearestRailroadCard.hpp"
#include "model/cards/HandCard.hpp"
#include "model/cards/LassoCard.hpp"
#include "model/cards/MoveBackThreeCard.hpp"
#include "model/cards/MoveCard.hpp"
#include "model/cards/ShieldCard.hpp"
#include "model/cards/TeleportCard.hpp"
#include "model/managers/LogManager.hpp"
#include "model/tiles/FestivalTile.hpp"
#include "model/tiles/GoTile.hpp"
#include "model/tiles/IncomeTaxTile.hpp"
#include "model/tiles/LuxuryTaxTile.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/RailroadTile.hpp"
#include "model/tiles/StreetTile.hpp"
#include "model/tiles/Tile.hpp"
#include "model/tiles/UtilityTile.hpp"
#include "view/raylib/UiToolkit.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <exception>
#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <vector>

using namespace view::raylibgui;

namespace {
UiToolkit toolkit;

Color playerAccent(int index) {
    switch (index) {
        case 0: return toolkit.theme().getCoral();
        case 1: return toolkit.theme().getTeal();
        case 2: return toolkit.theme().getGold();
        default: return toolkit.theme().getNavy();
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
    if (key == "ABUABU" || key == "UTILITY") return toolkit.theme().getTeal();
    return toolkit.colorForGroup(group);
}

Color kindAccent(TileKind kind, const std::string& group) {
    if (kind == TileKind::Street) return groupAccent(group);
    if (kind == TileKind::Railroad) return toolkit.theme().getNavy();
    if (kind == TileKind::Utility) return toolkit.theme().getTeal();
    if (kind == TileKind::Chance) return toolkit.theme().getCoral();
    if (kind == TileKind::CommunityChest) return toolkit.theme().getSage();
    if (kind == TileKind::IncomeTax || kind == TileKind::LuxuryTax) return toolkit.theme().getDanger();
    if (kind == TileKind::Festival) return toolkit.theme().getGold();
    if (kind == TileKind::Jail || kind == TileKind::GoToJail) return toolkit.theme().getDanger();
    if (kind == TileKind::Go) return toolkit.theme().getGold();
    return toolkit.theme().getPaper();
}

std::string tileFlavor(TileKind kind) {
    switch (kind) {
        case TileKind::Go: return "Petak mulai. Pemain mendapat bonus saat melewati GO.";
        case TileKind::Street: return "Properti street yang dapat dibeli, dibangun, digadai, dan menarik sewa.";
        case TileKind::Railroad: return "Properti stasiun dengan sewa berdasarkan jumlah stasiun yang dimiliki.";
        case TileKind::Utility: return "Properti utilitas dengan sewa berdasarkan hasil dadu.";
        case TileKind::Chance: return "Ambil kartu kesempatan dari deck backend.";
        case TileKind::CommunityChest: return "Ambil kartu dana umum dari deck backend.";
        case TileKind::IncomeTax: return "Bayar pajak penghasilan.";
        case TileKind::LuxuryTax: return "Bayar pajak barang mewah.";
        case TileKind::Festival: return "Aktifkan festival pada properti milik pemain.";
        case TileKind::Jail: return "Petak penjara atau kunjungan.";
        case TileKind::GoToJail: return "Pemain dikirim ke penjara.";
        case TileKind::FreeParking: return "Petak bebas parkir.";
    }
    return "";
}

bool hasTxtExtension(const std::string& filename) {
    if (filename.size() < 4) {
        return false;
    }
    std::string suffix = filename.substr(filename.size() - 4);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return suffix == ".txt";
}

std::string withTxtExtension(std::string filename) {
    if (filename.empty()) {
        filename = "save-1";
    }
    return hasTxtExtension(filename) ? filename : filename + ".txt";
}

}

GUIGameController::GUIGameController()
    : cardController_(*this),
      commandController_(*this),
      tileController_(*this) {
    appState_.setRng(std::mt19937(static_cast<unsigned int>(std::time(nullptr))));
    appState_.setSaveSlots(createInitialSaveSlots());
}

AppState& GUIGameController::state() {
    return appState_;
}

const AppState& GUIGameController::state() const {
    return appState_;
}

GUICardController& GUIGameController::cards() {
    return cardController_;
}

const GUICardController& GUIGameController::cards() const {
    return cardController_;
}

GUICommandController& GUIGameController::commands() {
    return commandController_;
}

const GUICommandController& GUIGameController::commands() const {
    return commandController_;
}

GUITileController& GUIGameController::tiles() {
    return tileController_;
}

const GUITileController& GUIGameController::tiles() const {
    return tileController_;
}

void GUIGameController::tick(float deltaTime) {
    appState_.setTime(appState_.getTime() + deltaTime);
    if (appState_.getOverlay().getType() == OverlayType::None) {
        appState_.getOverlay().setAnim(0.0f);
    } else {
        appState_.getOverlay().setAnim(std::min(1.0f, appState_.getOverlay().getAnim() + deltaTime * 8.0f));
    }
    updateToasts(deltaTime);
}

void GUIGameController::handleGlobalShortcuts() {
    if (IsKeyPressed(KEY_ESCAPE) && isOverlayOpen()) {
        closeOverlay();
        return;
    }

    if (appState_.getScreen() != Screen::Gameplay || isOverlayOpen()) {
        return;
    }

    if (IsKeyPressed(KEY_S)) {
        startTurn();
    } else if (IsKeyPressed(KEY_R)) {
        rollDice();
    } else if (IsKeyPressed(KEY_D)) {
        openSetDice();
    } else if (IsKeyPressed(KEY_E)) {
        endTurn();
    } else if (IsKeyPressed(KEY_P)) {
        openPortfolio();
    } else if (IsKeyPressed(KEY_C)) {
        openCards();
    } else if (IsKeyPressed(KEY_L)) {
        openLogs();
    } else if (IsKeyPressed(KEY_H)) {
        openHelp();
    }
}

void GUIGameController::openOverlay(OverlayType type) {
    appState_.getOverlay().setType(type);
    appState_.getOverlay().setAnim(0.0f);
    appState_.setActiveField(type == OverlayType::Save ? "save-name" : "");
}

void GUIGameController::closeOverlay() {
    closeCardDrawOverlay(true);
    appState_.setOverlay(OverlayState{});
    appState_.getActiveField().clear();
}

bool GUIGameController::isOverlayOpen() const {
    return appState_.getOverlay().getType() != OverlayType::None;
}

int GUIGameController::getMortgageValue(const TileInfo& tile) const {
    const OwnableTile* ownable = ownableFromUi(tile.getIndex());
    return ownable != nullptr ? ownable->getMortgageValue() : tile.getMortgageValue();
}

int GUIGameController::getRedeemCost(const TileInfo& tile) const {
    const OwnableTile* ownable = ownableFromUi(tile.getIndex());
    return ownable != nullptr ? backendGame_.getPropertyManager().getRedeemCost(*ownable) : getMortgageValue(tile);
}

int GUIGameController::findJailIndex() const {
    for (const std::shared_ptr<Tile>& tile : backendGame_.getBoard().getTiles()) {
        if (tile != nullptr && tile->onLand() == Tile::TileType::Jail) {
            return uiTileIndexFromBackend(tile->getIndex());
        }
    }
    return 0;
}

int GUIGameController::computeRent(const TileInfo& tile) const {
    const OwnableTile* ownable = ownableFromUi(tile.getIndex());
    if (ownable == nullptr || ownable->getOwner() == nullptr || ownable->isMortgaged()) {
        return 0;
    }

    RentContext rentContext = backendGame_.getPropertyManager().createRentContext(
        backendGame_.getBoard(),
        backendGame_.getConfigManager(),
        backendGame_.getDice(),
        *ownable
    );
    return ownable->calculateRent(rentContext);
}

int GUIGameController::computeTileAssetValue(const TileInfo& tile) const {
    const OwnableTile* ownable = ownableFromUi(tile.getIndex());
    if (ownable == nullptr) {
        return 0;
    }
    return ownable->getPurchasePrice() + ownable->getBuildingValue();
}

int GUIGameController::computeNetWorth(int playerIndex) const {
    const std::vector<Player>& players = backendGame_.getPlayers();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return 0;
    }

    int total = players.at(playerIndex).getMoney();
    for (OwnableTile* property : backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), players.at(playerIndex))) {
        if (property != nullptr) {
            total += property->getPurchasePrice() + property->getBuildingValue();
        }
    }
    total += players.at(playerIndex).countCards() * 75;
    return total;
}

int GUIGameController::jailFineAmount() const {
    return backendGame_.getConfigManager().getJailFine();
}

Rectangle GUIGameController::boardTileRect(Rectangle square, int index) const {
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

void GUIGameController::setSelectedTile(int tileIndex) {
    const int boardSize = static_cast<int>(appState_.getGame().getBoard().size());
    if (boardSize <= 0) {
        appState_.getGame().setSelectedTile(0);
        return;
    }
    appState_.getGame().setSelectedTile(std::max(0, std::min(tileIndex, boardSize - 1)));
}

void GUIGameController::openTileDetail(int tileIndex) {
    appState_.getOverlay().setTileIndex(tileIndex);
    openOverlay(OverlayType::TileDetail);
}

void GUIGameController::openPurchase(int tileIndex) {
    appState_.getOverlay().setTileIndex(tileIndex);
    openOverlay(OverlayType::Purchase);
}

void GUIGameController::openAuctionForTile(int tileIndex) {
    appState_.getOverlay().setTileIndex(tileIndex);

    if (StreetTile* street = streetFromUi(tileIndex)) {
        try {
            backendGame_.getAuctionManager().initializeAuction(
                backendGame_.getGameContext(),
                *street,
                backendGame_.getPlayers().empty() ? nullptr : &backendGame_.getCurrentPlayer()
            );
        } catch (const std::exception& exception) {
            addToast(exception.what(), RED);
        }
    }

    appState_.getOverlay().setAuction({
        tileIndex,
        appState_.getGame().getCurrentPlayer(),
        0,
        -1,
        std::vector<bool>(appState_.getGame().getPlayers().size(), false),
    });
    openOverlay(OverlayType::Auction);
}

void GUIGameController::openIncomeTax() {
    for (const TileInfo& tile : appState_.getGame().getBoard()) {
        if (tile.getKind() == TileKind::IncomeTax) {
            appState_.getOverlay().setTileIndex(tile.getIndex());
            break;
        }
    }
    openOverlay(OverlayType::IncomeTax);
}

void GUIGameController::openLuxuryTax() {
    for (const TileInfo& tile : appState_.getGame().getBoard()) {
        if (tile.getKind() == TileKind::LuxuryTax) {
            appState_.getOverlay().setTileIndex(tile.getIndex());
            break;
        }
    }
    openOverlay(OverlayType::LuxuryTax);
}

void GUIGameController::openFestival() {
    for (const TileInfo& tile : appState_.getGame().getBoard()) {
        if (tile.getKind() == TileKind::Festival) {
            appState_.getOverlay().setTileIndex(tile.getIndex());
            break;
        }
    }
    appState_.getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::Festival);
}

void GUIGameController::openJail() {
    appState_.getOverlay().setTileIndex(findJailIndex());
    openOverlay(OverlayType::Jail);
}

void GUIGameController::openCards() {
    appState_.getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::Cards);
}

void GUIGameController::openPortfolio() {
    openOverlay(OverlayType::Portfolio);
}

void GUIGameController::openBuild() {
    appState_.getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::Build);
}

void GUIGameController::openMortgage() {
    appState_.getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::Mortgage);
}

void GUIGameController::openRedeem() {
    appState_.getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::Redeem);
}

void GUIGameController::openSave() {
    openOverlay(OverlayType::Save);
}

void GUIGameController::openLogs() {
    openOverlay(OverlayType::Logs);
}

void GUIGameController::openSetDice() {
    appState_.getOverlay().setManualDieOne(std::max(1, appState_.getGame().getLastDieOne()));
    appState_.getOverlay().setManualDieTwo(std::max(1, appState_.getGame().getLastDieTwo()));
    openOverlay(OverlayType::SetDice);
}

void GUIGameController::openHelp() {
    openOverlay(OverlayType::Help);
}

void GUIGameController::openForceDrop() {
    appState_.getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::ForceDrop);
}

void GUIGameController::openLiquidation() {
    appState_.getOverlay().setSelectedIndex(0);
    appState_.getOverlay().setSelectedPlayer(appState_.getGame().getCurrentPlayer());
    openOverlay(OverlayType::Liquidation);
}

void GUIGameController::openRandomCardDraw(int deckKey) {
    clearPendingDrawnCard(true);
    try {
        if (deckKey == kCommunityDeckKey) {
            pendingCommunityChestCard_ = backendGame_.getCardManager().drawCommunityChestCard();
            if (pendingCommunityChestCard_ == nullptr) {
                addToast("Deck Dana Umum kosong.", RED);
                return;
            }
            appState_.getOverlay().setCard(makeCardInfoFromBackend(*pendingCommunityChestCard_));
        } else {
            pendingChanceCard_ = backendGame_.getCardManager().drawChanceCard();
            if (pendingChanceCard_ == nullptr) {
                addToast("Deck Chance kosong.", RED);
                return;
            }
            appState_.getOverlay().setCard(makeCardInfoFromBackend(*pendingChanceCard_));
        }
        appState_.getOverlay().setDeckKey(deckKey);
        openOverlay(OverlayType::CardDraw);
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        clearPendingDrawnCard(true);
    }
}

void GUIGameController::openGameOver() {
    openOverlay(OverlayType::GameOver);
}

void GUIGameController::startFreshSession() {
    try {
        closeOverlay();
        backendGame_ = Game();
        backendGame_.getConfigManager().loadAllConfigs();

        std::vector<Player>& players = backendGame_.getPlayers();
        players.clear();
        const int playerCount = std::max(kMinPlayers, std::min(kMaxPlayers, appState_.getPlayerCount()));
        const int startingCash = appState_.getStartingCash() > 0
            ? appState_.getStartingCash()
            : backendGame_.getConfigManager().getInitialBalance();

        for (int index = 0; index < playerCount; index++) {
            players.push_back(Player(
                appState_.getPlayerNames().at(index),
                startingCash,
                1,
                PlayerStatus::ACTIVE,
                0,
                false,
                false,
                0,
                0
            ));
        }

        backendGame_.startNewGame();
        if (appState_.getTurnLimit() > 0) {
            backendGame_.getGameContext().setMaxTurn(appState_.getTurnLimit());
        }

        guiTurnStarted_ = false;
        diceRolledThisTurn_ = false;
        appState_.setScreen(Screen::Gameplay);
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
    }
}

void GUIGameController::loadSessionFromSlot(int slotIndex) {
    appState_.setSelectedSave(slotIndex);
    std::string filename = appState_.getLoadInput();
    if (filename.empty() && slotIndex >= 0 && slotIndex < static_cast<int>(appState_.getSaveSlots().size())) {
        filename = appState_.getSaveSlots().at(slotIndex).getName();
    }
    filename = withTxtExtension(filename);

    try {
        closeOverlay();
        backendGame_ = Game();
        backendGame_.loadGame(filename);
        guiTurnStarted_ = true;
        diceRolledThisTurn_ = false;
        appState_.setScreen(Screen::Gameplay);
        syncViewFromBackend();
        addToast("Permainan dimuat dari " + filename + ".", SKYBLUE);
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
    }
}

void GUIGameController::startTurn() {
    if (appState_.getScreen() != Screen::Gameplay || !backendGame_.isGameRunning() || backendGame_.getPlayers().empty()) {
        return;
    }
    if (guiTurnStarted_) {
        addToast("Turn sudah berjalan.", RED);
        return;
    }

    try {
        backendGame_.getTurnManager().startTurn(backendGame_.getGameContext());
        Player& player = backendGame_.getCurrentPlayer();
        if (player.isBankrupt()) {
            addToast("Pemain ini sudah bangkrut.", RED);
            syncViewFromBackend();
            return;
        }

        player.setUsedHandCardThisTurn(false);
        guiTurnStarted_ = true;
        diceRolledThisTurn_ = false;
        addLog(player.getUsername(), "START_TURN", "Memulai giliran.");

        std::shared_ptr<HandCard> drawnCard = backendGame_.getCardManager().giveStartTurnCard(player);
        syncViewFromBackend();
        addToast("Giliran " + player.getUsername() + " dimulai.", playerAccent(currentBackendPlayerIndex()));
        if (drawnCard != nullptr) {
            addToast("Kartu baru: " + drawnCard->getName() + ".", SKYBLUE);
        }

        if (backendGame_.getCardManager().needsForceDrop(player)) {
            openForceDrop();
        } else if (player.isJailed()) {
            openJail();
        }
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::rollDice() {
    if (appState_.getScreen() != Screen::Gameplay || !backendGame_.isGameRunning() || backendGame_.getPlayers().empty()) {
        return;
    }
    if (!guiTurnStarted_) {
        addToast("Mulai turn dulu sebelum melempar dadu.", RED);
        return;
    }
    if (backendGame_.getTurnManager().isRolledThisTurn()) {
        addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    try {
        Player& player = backendGame_.getCurrentPlayer();
        if (player.isJailed()) {
            openJail();
            addToast("Pemain masih di jail.", RED);
            return;
        }

        backendGame_.getDice().roll();
        backendGame_.getTurnManager().registerDiceResult(backendGame_.getDice().isDouble());
        diceRolledThisTurn_ = true;

        if (backendGame_.getTurnManager().getConsecutiveDoubles() >= 3) {
            backendGame_.getJailManager().sendToJail(player);
            player.moveTo(backendTileIndexFromUi(findJailIndex()));
            addLog(player.getUsername(), "JAIL", "Masuk penjara karena double tiga kali berturut-turut.");
            syncViewFromBackend();
            openJail();
            return;
        }

        const int total = backendGame_.getDice().getTotal();
        const int destination = moveBackendPlayer(player, total);
        addLog(
            player.getUsername(),
            "DADU",
            "Lempar " + std::to_string(backendGame_.getDice().getDie1()) + "+" +
                std::to_string(backendGame_.getDice().getDie2()) + "=" + std::to_string(total)
        );
        addToast(
            "Dadu " + std::to_string(backendGame_.getDice().getDie1()) + " + " +
                std::to_string(backendGame_.getDice().getDie2()),
            playerAccent(currentBackendPlayerIndex())
        );
        resolveBackendLanding(destination, false);
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::applyManualDice() {
    if (appState_.getScreen() != Screen::Gameplay || !backendGame_.isGameRunning() || backendGame_.getPlayers().empty()) {
        return;
    }
    if (!guiTurnStarted_) {
        addToast("Mulai turn dulu sebelum mengatur dadu.", RED);
        return;
    }
    if (backendGame_.getTurnManager().isRolledThisTurn()) {
        addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    try {
        Player& player = backendGame_.getCurrentPlayer();
        if (player.isJailed()) {
            openJail();
            addToast("Pemain masih di jail.", RED);
            return;
        }

        const int dieOne = appState_.getOverlay().getManualDieOne();
        const int dieTwo = appState_.getOverlay().getManualDieTwo();
        backendGame_.getDice() = Dice(dieOne, dieTwo, true);
        backendGame_.getTurnManager().registerDiceResult(backendGame_.getDice().isDouble());
        diceRolledThisTurn_ = true;
        closeOverlay();

        if (backendGame_.getTurnManager().getConsecutiveDoubles() >= 3) {
            backendGame_.getJailManager().sendToJail(player);
            player.moveTo(backendTileIndexFromUi(findJailIndex()));
            addLog(player.getUsername(), "JAIL", "Masuk penjara karena double tiga kali berturut-turut.");
            syncViewFromBackend();
            openJail();
            return;
        }

        const int destination = moveBackendPlayer(player, dieOne + dieTwo);
        addLog(
            player.getUsername(),
            "ATUR_DADU",
            "Dadu manual " + std::to_string(dieOne) + "+" + std::to_string(dieTwo)
        );
        addToast("Dadu manual diterapkan.", playerAccent(currentBackendPlayerIndex()));
        resolveBackendLanding(destination, false);
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::endTurn() {
    if (!backendGame_.isGameRunning() || backendGame_.getPlayers().empty()) {
        return;
    }

    try {
        Player& player = backendGame_.getCurrentPlayer();
        if (player.getMoney() < 0 || backendGame_.getBankruptcyManager().isBankruptcyActive()) {
            syncViewFromBackend();
            maybeOpenLiquidation();
            return;
        }

        player.setShieldActive(false);
        player.decrementDiscountDuration();
        const int previousTurn = backendGame_.getCurrentTurn();
        backendGame_.getTurnManager().endTurn(backendGame_.getGameContext());
        if (backendGame_.getCurrentTurn() > previousTurn) {
            std::vector<OwnableTile*> properties;
            for (const std::shared_ptr<Tile>& tile : backendGame_.getBoard().getTiles()) {
                std::shared_ptr<OwnableTile> ownable = std::dynamic_pointer_cast<OwnableTile>(tile);
                if (ownable != nullptr) {
                    properties.push_back(ownable.get());
                }
            }
            backendGame_.getFestivalManager().decrementFestivalDurations(properties);
        }

        guiTurnStarted_ = false;
        diceRolledThisTurn_ = false;
        syncViewFromBackend();
        if (backendGame_.isGameOver()) {
            openGameOver();
        }
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::triggerTileEvent(int tileIndex, bool fromMovement) {
    resolveBackendLanding(backendTileIndexFromUi(tileIndex), fromMovement);
}

bool GUIGameController::canSaveNow() const {
    return backendGame_.isGameRunning() && guiTurnStarted_ && !backendGame_.getTurnManager().isRolledThisTurn();
}

void GUIGameController::adjustManualDie(int dieIndex, int delta) {
    OverlayState& overlay = appState_.getOverlay();
    const int die = dieIndex == 1 ? overlay.getManualDieOne() : overlay.getManualDieTwo();
    const int adjusted = std::max(1, std::min(6, die + delta));
    if (dieIndex == 1) {
        overlay.setManualDieOne(adjusted);
    } else {
        overlay.setManualDieTwo(adjusted);
    }
}

bool GUIGameController::canCurrentPlayerAffordSelectedPurchase() const {
    const int playerIndex = currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return false;
    }
    return backendGame_.getPlayers().at(playerIndex).getMoney() >= currentPurchasePrice();
}

int GUIGameController::currentPurchasePrice() const {
    const OwnableTile* ownable = ownableFromUi(appState_.getOverlay().getTileIndex());
    const int playerIndex = currentBackendPlayerIndex();
    if (ownable == nullptr || playerIndex < 0 || playerIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return 0;
    }
    return backendGame_.getPlayers().at(playerIndex).effectiveCost(ownable->getPurchasePrice());
}

void GUIGameController::buySelectedProperty() {
    try {
        OwnableTile* property = ownableFromUi(appState_.getOverlay().getTileIndex());
        if (property == nullptr || property->getOwner() != nullptr || property->getOwnershipStatus() != OwnershipStatus::BANK) {
            addToast("Properti ini tidak bisa dibeli.", RED);
            return;
        }

        Player& player = backendGame_.getCurrentPlayer();
        const int price = player.effectiveCost(property->getPurchasePrice());
        if (player.getMoney() < price) {
            addToast("Saldo tidak cukup untuk membeli properti.", RED);
            return;
        }

        player -= property->getPurchasePrice();
        property->setOwner(&player);
        property->setOwnershipStatus(OwnershipStatus::OWNED);
        addLog(player.getUsername(), "BELI", "Membeli " + property->getName() + " seharga M" + std::to_string(price) + ".");
        addToast("Berhasil membeli " + property->getName() + ".", groupAccent(""));
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::skipSelectedPurchase() {
    closeOverlay();
}

void GUIGameController::auctionRaiseBid(int amount) {
    AuctionState& auction = appState_.getOverlay().getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return;
    }
    if (bidderIndex < static_cast<int>(auction.getPassed().size()) && auction.getPassed().at(bidderIndex)) {
        addToast("Bidder ini sudah pass.", RED);
        return;
    }

    Player& bidder = backendGame_.getPlayers().at(bidderIndex);
    const int nextBid = auction.getHighestBid() + amount;
    if (bidder.isBankrupt() || bidder.getMoney() < nextBid) {
        addToast("Bidder tidak bisa menaikkan bid.", RED);
        return;
    }

    backendGame_.getAuctionManager().placeBid(bidder, nextBid);
    auction.setHighestBid(nextBid);
    auction.setHighestBidder(bidderIndex);
    addLog(bidder.getUsername(), "BID", "Menawar M" + std::to_string(nextBid) + ".");

    for (int checked = 0; checked < static_cast<int>(backendGame_.getPlayers().size()); checked++) {
        const int nextIndex = (bidderIndex + checked + 1) % static_cast<int>(backendGame_.getPlayers().size());
        if (nextIndex < static_cast<int>(auction.getPassed().size()) &&
            !auction.getPassed().at(nextIndex) &&
            !backendGame_.getPlayers().at(nextIndex).isBankrupt()) {
            auction.setSelectedBidder(nextIndex);
            break;
        }
    }
    syncViewFromBackend();
}

void GUIGameController::auctionPass() {
    AuctionState& auction = appState_.getOverlay().getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return;
    }

    if (bidderIndex < static_cast<int>(auction.getPassed().size())) {
        auction.getPassed().at(bidderIndex) = true;
        backendGame_.getAuctionManager().pass(backendGame_.getPlayers().at(bidderIndex));
        addLog(backendGame_.getPlayers().at(bidderIndex).getUsername(), "PASS", "Pass pada lelang.");
    }

    for (int checked = 0; checked < static_cast<int>(backendGame_.getPlayers().size()); checked++) {
        const int nextIndex = (bidderIndex + checked + 1) % static_cast<int>(backendGame_.getPlayers().size());
        if (nextIndex < static_cast<int>(auction.getPassed().size()) &&
            !auction.getPassed().at(nextIndex) &&
            !backendGame_.getPlayers().at(nextIndex).isBankrupt()) {
            auction.setSelectedBidder(nextIndex);
            return;
        }
    }
}

void GUIGameController::finalizeAuction() {
    try {
        AuctionState& auction = appState_.getOverlay().getAuction();
        OwnableTile* property = ownableFromUi(auction.getTileIndex());
        const int winnerIndex = auction.getHighestBidder();
        if (property != nullptr &&
            winnerIndex >= 0 &&
            winnerIndex < static_cast<int>(backendGame_.getPlayers().size())) {
            Player& winner = backendGame_.getPlayers().at(winnerIndex);
            if (!winner.isBankrupt() && winner.getMoney() >= auction.getHighestBid()) {
                winner -= auction.getHighestBid();
                property->setOwner(&winner);
                property->setOwnershipStatus(OwnershipStatus::OWNED);
                addLog(
                    winner.getUsername(),
                    "LELANG",
                    "Menang " + property->getName() + " seharga M" + std::to_string(auction.getHighestBid()) + "."
                );
                addToast("Lelang dimenangkan " + winner.getUsername() + ".", playerAccent(winnerIndex));
            }
        }
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

int GUIGameController::flatIncomeTax() const {
    return backendGame_.getConfigManager().getPphFlat();
}

int GUIGameController::percentageIncomeTax() const {
    const int playerIndex = currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return 0;
    }
    const int wealth = computeNetWorth(playerIndex);
    return wealth * backendGame_.getConfigManager().getPphPercent() / 100;
}

void GUIGameController::payIncomeTax(bool useFlatTax) {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int tax = useFlatTax ? flatIncomeTax() : percentageIncomeTax();
        if (player.isShieldActive()) {
            player.setShieldActive(false);
            addToast("Shield menahan pajak.", playerAccent(currentBackendPlayerIndex()));
        } else if (player.getMoney() < player.effectiveCost(tax)) {
            backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, tax, true);
            addToast("Dana kurang untuk membayar pajak.", RED);
        } else {
            backendGame_.getTaxManager().processTaxPayment(player, tax);
        }
        addLog(player.getUsername(), "PAJAK", "Membayar PPh M" + std::to_string(tax) + ".");
        closeOverlay();
        syncViewFromBackend();
        maybeOpenLiquidation();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
        maybeOpenLiquidation();
    }
}

int GUIGameController::luxuryTaxAmount() const {
    return backendGame_.getConfigManager().getPbmFlat();
}

void GUIGameController::payLuxuryTax() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int tax = luxuryTaxAmount();
        if (player.isShieldActive()) {
            player.setShieldActive(false);
            addToast("Shield menahan pajak.", playerAccent(currentBackendPlayerIndex()));
        } else if (player.getMoney() < player.effectiveCost(tax)) {
            backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, tax, true);
            addToast("Dana kurang untuk membayar pajak.", RED);
        } else {
            backendGame_.getTaxManager().processTaxPayment(player, tax);
        }
        addLog(player.getUsername(), "PAJAK", "Membayar PBM M" + std::to_string(tax) + ".");
        closeOverlay();
        syncViewFromBackend();
        maybeOpenLiquidation();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
        maybeOpenLiquidation();
    }
}

void GUIGameController::activateFestivalOnSelectedTile() {
    const std::vector<int> options = currentPlayerStreetOptions();
    if (options.empty()) {
        addToast("Belum ada street untuk festival.", RED);
        return;
    }

    try {
        const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
        OwnableTile* property = ownableFromUi(options.at(selected));
        if (property == nullptr) {
            addToast("Target festival tidak valid.", RED);
            return;
        }

        backendGame_.getFestivalManager().activateFestival(*property);
        addLog(backendGame_.getCurrentPlayer().getUsername(), "FESTIVAL", "Mengaktifkan festival di " + property->getName() + ".");
        addToast("Festival aktif di " + property->getName() + ".", kindAccent(TileKind::Festival, ""));
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::payJailFine() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int fine = jailFineAmount();
        if (player.getMoney() < player.effectiveCost(fine)) {
            backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, fine, true);
            syncViewFromBackend();
            openLiquidation();
            return;
        }

        backendGame_.getJailManager().payJailFine(player, fine);
        backendGame_.getJailManager().releaseFromJail(player);
        addLog(player.getUsername(), "JAIL", "Membayar denda M" + std::to_string(fine) + ".");
        addToast("Denda jail dibayar.", playerAccent(currentBackendPlayerIndex()));
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::useJailCard() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        std::vector<std::shared_ptr<HandCard>> cards = backendGame_.getCardManager().getHandCards(player);
        int shieldIndex = -1;
        for (int index = 0; index < static_cast<int>(cards.size()); index++) {
            if (std::dynamic_pointer_cast<ShieldCard>(cards.at(index)) != nullptr) {
                shieldIndex = index;
                break;
            }
        }

        if (shieldIndex < 0) {
            addToast("Tidak ada kartu shield untuk keluar jail.", RED);
            return;
        }

        backendGame_.getCardManager().dropHandCard(player, shieldIndex);
        backendGame_.getJailManager().releaseFromJail(player);
        addLog(player.getUsername(), "JAIL", "Menggunakan ShieldCard untuk keluar dari jail.");
        addToast("Keluar dari jail dengan kartu.", SKYBLUE);
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::useSelectedHandCard() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        std::vector<std::shared_ptr<HandCard>> cards = backendGame_.getCardManager().getHandCards(player);
        if (cards.empty()) {
            addToast("Belum ada kartu di tangan.", RED);
            return;
        }
        if (player.hasUsedHandCardThisTurn()) {
            addToast("Kartu kemampuan sudah dipakai pada turn ini.", RED);
            return;
        }

        const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(cards.size()) - 1));
        const int previousPosition = player.getPosition();
        configureSelectedHandCard(player, selected);
        backendGame_.getCardManager().useHandCard(backendGame_, player, selected);

        const int normalizedPosition = normalizedBackendTileIndex(player.getPosition());
        player.moveTo(normalizedPosition);
        addToast("Kartu digunakan.", SKYBLUE);
        closeOverlay();
        if (normalizedPosition != normalizedBackendTileIndex(previousPosition)) {
            resolveBackendLanding(normalizedPosition, true);
        } else {
            syncViewFromBackend();
        }
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::storeDrawnCard() {
    addToast("Kartu backend dari petak ini harus langsung diterapkan.", RED);
}

void GUIGameController::applyDrawnCard() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int previousPosition = player.getPosition();
        if (pendingChanceCard_ != nullptr) {
            addLog(player.getUsername(), "KSP", "Mengambil kartu " + pendingChanceCard_->getName() + ".");
            pendingChanceCard_->apply(backendGame_, player);
            backendGame_.getCardManager().discardChanceCard(pendingChanceCard_);
            pendingChanceCard_.reset();
        } else if (pendingCommunityChestCard_ != nullptr) {
            addLog(player.getUsername(), "DNU", "Mengambil kartu " + pendingCommunityChestCard_->getName() + ".");
            pendingCommunityChestCard_->apply(backendGame_, player);
            backendGame_.getCardManager().discardCommunityChestCard(pendingCommunityChestCard_);
            pendingCommunityChestCard_.reset();
        } else {
            closeOverlay();
            return;
        }

        const int normalizedPosition = normalizedBackendTileIndex(player.getPosition());
        player.moveTo(normalizedPosition);
        closeOverlay();
        if (normalizedPosition != normalizedBackendTileIndex(previousPosition)) {
            resolveBackendLanding(normalizedPosition, true);
        } else {
            syncViewFromBackend();
            maybeOpenLiquidation();
        }
    } catch (const std::exception& exception) {
        clearPendingDrawnCard(true);
        addToast(exception.what(), RED);
        syncViewFromBackend();
        maybeOpenLiquidation();
    }
}

void GUIGameController::buildOnSelectedTile() {
    const std::vector<int> options = currentPlayerBuildOptions();
    if (options.empty()) {
        addToast("Tidak ada properti yang bisa dibangun.", RED);
        return;
    }

    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
        StreetTile* street = streetFromUi(options.at(selected));
        if (street == nullptr) {
            addToast("Target bangunan tidak valid.", RED);
            return;
        }
        const int oldLevel = street->getBuildingLevel();
        backendGame_.getPropertyManager().buildOnStreet(backendGame_.getBoard(), player, *street);
        if (street->getBuildingLevel() == oldLevel) {
            addToast("Properti ini belum memenuhi syarat bangun.", RED);
            return;
        }
        addLog(player.getUsername(), "BANGUN", "Menambah bangunan di " + street->getName() + ".");
        addToast("Bangunan ditambahkan di " + street->getName() + ".", groupAccent(street->getColorGroup()));
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::mortgageSelectedTile() {
    const std::vector<int> options = currentPlayerMortgageOptions();
    if (options.empty()) {
        addToast("Tidak ada aset yang bisa digadai.", RED);
        return;
    }

    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
        OwnableTile* property = ownableFromUi(options.at(selected));
        if (property == nullptr) {
            addToast("Aset tidak valid.", RED);
            return;
        }
        backendGame_.getPropertyManager().mortgageProperty(player, *property);
        addLog(player.getUsername(), "GADAI", "Menggadaikan " + property->getName() + ".");
        addToast("Aset digadai.", kindAccent(toGuiTileKind(*property), ""));
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::redeemSelectedTile() {
    const std::vector<int> options = currentPlayerRedeemOptions();
    if (options.empty()) {
        addToast("Tidak ada aset tergadai untuk ditebus.", RED);
        return;
    }

    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
        OwnableTile* property = ownableFromUi(options.at(selected));
        if (property == nullptr) {
            addToast("Aset tidak valid.", RED);
            return;
        }
        if (player.getMoney() < player.effectiveCost(backendGame_.getPropertyManager().getRedeemCost(*property))) {
            addToast("Saldo tidak cukup untuk menebus aset.", RED);
            return;
        }
        backendGame_.getPropertyManager().redeemProperty(player, *property);
        addLog(player.getUsername(), "TEBUS", "Menebus " + property->getName() + ".");
        addToast("Aset berhasil ditebus.", kindAccent(toGuiTileKind(*property), ""));
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::saveSession() {
    if (!canSaveNow()) {
        addToast("Game hanya bisa disimpan di awal turn.", RED);
        return;
    }

    std::string filename = withTxtExtension(appState_.getSaveInput());
    try {
        backendGame_.getSaveLoadManager().saveGame(backendGame_, filename);

        SaveSlot slot;
        slot.setName(filename);
        slot.setSubtitle(
            backendGame_.getPlayers().empty()
                ? "Belum ada pemain"
                : backendGame_.getCurrentPlayer().getUsername() + " - " +
                      std::to_string(backendGame_.getCurrentPlayer().getMoney())
        );
        slot.setTurn(backendGame_.getCurrentTurn());
        slot.setPlayerCount(static_cast<int>(backendGame_.getPlayers().size()));
        slot.setAccent(playerAccent(currentBackendPlayerIndex()));

        std::vector<SaveSlot>& saveSlots = appState_.getSaveSlots();
        int slotIndex = appState_.getSelectedSave();
        if (slotIndex < 0 || slotIndex >= static_cast<int>(saveSlots.size())) {
            saveSlots.push_back(slot);
            appState_.setSelectedSave(static_cast<int>(saveSlots.size()) - 1);
        } else {
            saveSlots.at(slotIndex) = slot;
        }

        addToast("Session tersimpan sebagai " + filename + ".", SKYBLUE);
        closeOverlay();
        syncViewFromBackend();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::dropSelectedHandCard() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        const int cardCount = player.countCards();
        if (cardCount <= 0) {
            closeOverlay();
            return;
        }

        const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), cardCount - 1));
        std::vector<std::shared_ptr<HandCard>> cards = backendGame_.getCardManager().getHandCards(player);
        std::string cardName = selected < static_cast<int>(cards.size()) && cards.at(selected) != nullptr
            ? cards.at(selected)->getName()
            : "Kartu";
        backendGame_.getCardManager().dropHandCard(player, selected);
        addLog(player.getUsername(), "DROP_CARD", "Membuang " + cardName + ".");
        addToast("Kartu dibuang.", SKYBLUE);
        syncViewFromBackend();
        if (!backendGame_.getCardManager().needsForceDrop(player)) {
            closeOverlay();
        }
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::liquidateSelectedTile() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        std::vector<OwnableTile*> properties = backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), player);
        if (properties.empty()) {
            addToast("Tidak ada aset untuk dijual.", RED);
            return;
        }

        const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(properties.size()) - 1));
        OwnableTile* property = properties.at(selected);
        const int saleValue = backendGame_.getPropertyManager().calculateSellToBankValue(*property);
        backendGame_.getPropertyManager().sellPropertyToBank(player, *property);
        property->setOwnershipStatus(OwnershipStatus::BANK);
        if (backendGame_.getBankruptcyManager().isBankruptcyActive()) {
            backendGame_.getBankruptcyManager().resolveLiquidationAction(player, "SELL");
        }
        addLog(player.getUsername(), "LIKUIDASI", "Menjual " + property->getName() + " ke bank.");
        addToast("Aset dijual seharga M" + std::to_string(saleValue) + ".", kindAccent(toGuiTileKind(*property), ""));
        syncViewFromBackend();
        if (!backendGame_.getBankruptcyManager().isBankruptcyActive()) {
            closeOverlay();
        }
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::declareBankrupt() {
    try {
        Player& player = backendGame_.getCurrentPlayer();
        std::vector<OwnableTile*> properties = backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), player);
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
        discardAllCards(player);
        backendGame_.getJailManager().releaseFromJail(player);
        backendGame_.getBankruptcyManager().declareBankrupt(player, nullptr);
        addLog(player.getUsername(), "BANGKRUT", "Pemain keluar dari permainan.");
        addToast(player.getUsername() + " bangkrut.", RED);
        closeOverlay();
        syncViewFromBackend();
        if (backendGame_.isGameOver()) {
            openGameOver();
        } else {
            endTurn();
        }
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
        syncViewFromBackend();
    }
}

void GUIGameController::syncViewFromBackend() {
    const GameState previous = appState_.getGame();
    GameState view;
    view.setSessionLabel("NIMONSPOLI");
    view.setToasts(previous.getToasts());
    view.setSelectedTile(previous.getSelectedTile());
    view.setHoveredTile(previous.getHoveredTile());
    view.setStartCash(appState_.getStartingCash());
    view.setTurnStarted(guiTurnStarted_);
    view.setRolledThisTurn(backendGame_.getTurnManager().isRolledThisTurn());
    view.setLastDieOne(backendGame_.getDice().getDie1());
    view.setLastDieTwo(backendGame_.getDice().getDie2());
    view.setTurn(backendGame_.getCurrentTurn());
    view.setTurnLimit(backendGame_.getMaxTurn());
    view.setCurrentPlayer(std::max(0, currentBackendPlayerIndex()));

    std::vector<TileInfo> board;
    for (const std::shared_ptr<Tile>& tile : backendGame_.getBoard().getTiles()) {
        if (tile != nullptr) {
            board.push_back(makeTileInfoFromBackend(*tile));
        }
    }
    std::sort(board.begin(), board.end(), [](const TileInfo& left, const TileInfo& right) {
        return left.getIndex() < right.getIndex();
    });
    view.setBoard(board);

    std::vector<PlayerInfo> players;
    const std::vector<Player>& backendPlayers = backendGame_.getPlayers();
    for (int index = 0; index < static_cast<int>(backendPlayers.size()); index++) {
        players.push_back(makePlayerInfoFromBackend(backendPlayers.at(index), index));
    }
    view.setPlayers(players);

    std::vector<LogItem> logs;
    const std::vector<LogManager::LogEntry>& backendLogs = backendGame_.getLogManager().getLogs();
    const int firstLog = std::max(0, static_cast<int>(backendLogs.size()) - kMaxLogEntries);
    for (int index = firstLog; index < static_cast<int>(backendLogs.size()); index++) {
        logs.push_back(makeLogItemFromBackend(backendLogs.at(index)));
    }
    view.setLogs(logs);

    if (!view.getBoard().empty()) {
        view.setSelectedTile(std::max(0, std::min(view.getSelectedTile(), static_cast<int>(view.getBoard().size()) - 1)));
    } else {
        view.setSelectedTile(0);
    }
    if (!view.getPlayers().empty()) {
        view.setCurrentPlayer(std::max(0, std::min(view.getCurrentPlayer(), static_cast<int>(view.getPlayers().size()) - 1)));
    } else {
        view.setCurrentPlayer(0);
    }

    appState_.setGame(view);
}

std::vector<int> GUIGameController::currentPlayerStreetOptions() const {
    std::vector<int> streets;
    const int playerIndex = currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return streets;
    }

    for (OwnableTile* property : backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), backendGame_.getPlayers().at(playerIndex))) {
        if (dynamic_cast<StreetTile*>(property) != nullptr) {
            streets.push_back(uiTileIndexFromBackend(property->getIndex()));
        }
    }
    return streets;
}

std::vector<int> GUIGameController::currentPlayerBuildOptions() const {
    std::vector<int> buildable;
    const int playerIndex = currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return buildable;
    }

    Player& player = const_cast<Player&>(backendGame_.getPlayers().at(playerIndex));
    for (OwnableTile* property : backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), player)) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street != nullptr && backendGame_.getPropertyManager().canBuild(backendGame_.getBoard(), player, *street)) {
            buildable.push_back(uiTileIndexFromBackend(street->getIndex()));
        }
    }
    return buildable;
}

std::vector<int> GUIGameController::currentPlayerMortgageOptions() const {
    std::vector<int> items;
    const int playerIndex = currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return items;
    }

    for (OwnableTile* property : backendGame_.getPropertyManager().getMortgageableProperties(backendGame_, backendGame_.getPlayers().at(playerIndex))) {
        if (property != nullptr) {
            items.push_back(uiTileIndexFromBackend(property->getIndex()));
        }
    }
    return items;
}

std::vector<int> GUIGameController::currentPlayerRedeemOptions() const {
    std::vector<int> items;
    const int playerIndex = currentBackendPlayerIndex();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(backendGame_.getPlayers().size())) {
        return items;
    }

    for (OwnableTile* property : backendGame_.getPropertyManager().getRedeemableProperties(backendGame_, backendGame_.getPlayers().at(playerIndex))) {
        if (property != nullptr) {
            items.push_back(uiTileIndexFromBackend(property->getIndex()));
        }
    }
    return items;
}

void GUIGameController::addToast(const std::string& text, Color accent, float duration) {
    std::deque<Toast>& toasts = appState_.getGame().getToasts();
    if (toasts.size() >= 4) {
        toasts.pop_front();
    }
    toasts.push_back({text, accent, duration, duration});
}

void GUIGameController::updateToasts(float deltaTime) {
    std::deque<Toast>& toasts = appState_.getGame().getToasts();
    for (Toast& toast : toasts) {
        toast.setTimeLeft(toast.getTimeLeft() - deltaTime);
    }
    while (!toasts.empty() && toasts.front().getTimeLeft() <= 0.0f) {
        toasts.pop_front();
    }
}

void GUIGameController::addLog(const std::string& actor, const std::string& action, const std::string& detail) {
    if (backendGame_.isGameRunning() || !backendGame_.getPlayers().empty()) {
        backendGame_.getLogManager().addLog(backendGame_.getCurrentTurn(), actor, action, detail);
        return;
    }

    GameState& game = appState_.getGame();
    game.getLogs().push_back({game.getTurn(), actor, action, detail});
    if (static_cast<int>(game.getLogs().size()) > kMaxLogEntries) {
        game.getLogs().erase(game.getLogs().begin());
    }
}

void GUIGameController::maybeOpenLiquidation() {
    if (!backendGame_.isGameRunning() || backendGame_.getPlayers().empty()) {
        return;
    }
    const Player& player = backendGame_.getCurrentPlayer();
    if (player.getMoney() < 0 || backendGame_.getBankruptcyManager().isBankruptcyActive()) {
        openLiquidation();
    }
}

int GUIGameController::uiTileIndexFromBackend(int backendIndex) const {
    const int boardSize = backendGame_.getBoard().getBoardSize();
    if (boardSize <= 0) {
        return 0;
    }
    return normalizedBackendTileIndex(backendIndex) - 1;
}

int GUIGameController::backendTileIndexFromUi(int uiIndex) const {
    const int boardSize = backendGame_.getBoard().getBoardSize();
    if (boardSize <= 0) {
        return 1;
    }
    int zeroBased = uiIndex % boardSize;
    if (zeroBased < 0) {
        zeroBased += boardSize;
    }
    return zeroBased + 1;
}

int GUIGameController::normalizedBackendTileIndex(int backendIndex) const {
    const int boardSize = backendGame_.getBoard().getBoardSize();
    if (boardSize <= 0) {
        return backendIndex;
    }
    int zeroBased = (backendIndex - 1) % boardSize;
    if (zeroBased < 0) {
        zeroBased += boardSize;
    }
    return zeroBased + 1;
}

int GUIGameController::backendPlayerIndex(const Player* player) const {
    const std::vector<Player>& players = backendGame_.getPlayers();
    for (int index = 0; index < static_cast<int>(players.size()); index++) {
        if (&players.at(index) == player) {
            return index;
        }
    }
    return -1;
}

int GUIGameController::currentBackendPlayerIndex() const {
    if (backendGame_.getPlayers().empty()) {
        return 0;
    }
    try {
        return backendPlayerIndex(&backendGame_.getCurrentPlayer());
    } catch (...) {
        return 0;
    }
}

Tile* GUIGameController::tileFromUi(int uiIndex) const {
    std::shared_ptr<Tile> tile = backendGame_.getBoard().getTile(backendTileIndexFromUi(uiIndex));
    return tile.get();
}

OwnableTile* GUIGameController::ownableFromUi(int uiIndex) const {
    return dynamic_cast<OwnableTile*>(tileFromUi(uiIndex));
}

StreetTile* GUIGameController::streetFromUi(int uiIndex) const {
    return dynamic_cast<StreetTile*>(tileFromUi(uiIndex));
}

TileKind GUIGameController::toGuiTileKind(const Tile& tile) const {
    switch (tile.onLand()) {
        case Tile::TileType::Go: return TileKind::Go;
        case Tile::TileType::Street: return TileKind::Street;
        case Tile::TileType::Railroad: return TileKind::Railroad;
        case Tile::TileType::Utility: return TileKind::Utility;
        case Tile::TileType::Chance: return TileKind::Chance;
        case Tile::TileType::CommunityChest: return TileKind::CommunityChest;
        case Tile::TileType::IncomeTax: return TileKind::IncomeTax;
        case Tile::TileType::LuxuryTax: return TileKind::LuxuryTax;
        case Tile::TileType::Festival: return TileKind::Festival;
        case Tile::TileType::Jail: return TileKind::Jail;
        case Tile::TileType::GoToJail: return TileKind::GoToJail;
        case Tile::TileType::FreeParking: return TileKind::FreeParking;
    }
    return TileKind::Go;
}

TileInfo GUIGameController::makeTileInfoFromBackend(const Tile& tile) const {
    const TileKind kind = toGuiTileKind(tile);
    std::string group;
    int price = 0;
    int mortgageValue = 0;
    int baseRent = 0;
    int houseCost = 0;
    int hotelCost = 0;
    int ownerIndex = -1;
    int buildings = 0;
    int festivalTurns = 0;
    bool mortgaged = false;

    const OwnableTile* ownable = dynamic_cast<const OwnableTile*>(&tile);
    if (ownable != nullptr) {
        price = ownable->getPurchasePrice();
        mortgageValue = ownable->getMortgageValue();
        ownerIndex = backendPlayerIndex(ownable->getOwner());
        festivalTurns = ownable->getFestivalDuration();
        mortgaged = ownable->isMortgaged();
    }

    const StreetTile* street = dynamic_cast<const StreetTile*>(&tile);
    if (street != nullptr) {
        group = street->getColorGroup();
        houseCost = street->getHouseBuildCost();
        hotelCost = street->getHotelBuildCost();
        buildings = street->getBuildingLevel();
        if (!street->getRentLevels().empty()) {
            baseRent = street->getRentLevels().front();
        }
    } else if (ownable != nullptr) {
        RentContext rentContext = backendGame_.getPropertyManager().createRentContext(
            backendGame_.getBoard(),
            backendGame_.getConfigManager(),
            backendGame_.getDice(),
            *ownable
        );
        baseRent = ownable->calculateRent(rentContext);
    }

    return {
        uiTileIndexFromBackend(tile.getIndex()),
        tile.getCode(),
        tile.getName(),
        tileFlavor(kind),
        group,
        kind,
        kindAccent(kind, group),
        price,
        mortgageValue,
        baseRent,
        houseCost,
        hotelCost,
        ownerIndex,
        buildings,
        festivalTurns,
        mortgaged
    };
}

PlayerInfo GUIGameController::makePlayerInfoFromBackend(const Player& player, int playerIndex) const {
    std::vector<int> properties;
    for (OwnableTile* property : backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), player)) {
        if (property != nullptr) {
            properties.push_back(uiTileIndexFromBackend(property->getIndex()));
        }
    }

    std::vector<CardInfo> cards;
    for (const std::shared_ptr<HandCard>& card : backendGame_.getCardManager().getHandCards(player)) {
        if (card != nullptr) {
            cards.push_back(makeCardInfoFromBackend(*card));
        }
    }

    return {
        player.getUsername(),
        playerAccent(playerIndex),
        player.getMoney(),
        uiTileIndexFromBackend(player.getPosition()),
        player.isJailed(),
        player.isBankrupt(),
        player.isShieldActive(),
        player.getFailedJailRolls(),
        player.getDiscountPercent(),
        properties,
        cards
    };
}

CardInfo GUIGameController::makeCardInfoFromBackend(const Card& card) const {
    CardKind kind = dynamic_cast<const HandCard*>(&card) != nullptr ? CardKind::Hand : CardKind::Instant;
    CardEffect effect = CardEffect::GainMoney;
    Color accent = SKYBLUE;
    int magnitude = 0;

    if (dynamic_cast<const DoctorFeeCard*>(&card) != nullptr ||
        dynamic_cast<const CampaignFeeCard*>(&card) != nullptr) {
        effect = CardEffect::LoseMoney;
        accent = RED;
    } else if (dynamic_cast<const GoToJailCard*>(&card) != nullptr) {
        effect = CardEffect::GoToJail;
        accent = RED;
    } else if (dynamic_cast<const MoveBackThreeCard*>(&card) != nullptr) {
        effect = CardEffect::MoveBackThree;
        magnitude = 3;
        accent = ORANGE;
    } else if (dynamic_cast<const GoToNearestRailroadCard*>(&card) != nullptr ||
               dynamic_cast<const MoveCard*>(&card) != nullptr) {
        effect = CardEffect::MoveToGo;
        accent = toolkit.theme().getGold();
    } else if (dynamic_cast<const TeleportCard*>(&card) != nullptr) {
        effect = CardEffect::TeleportAnywhere;
        accent = SKYBLUE;
    } else if (dynamic_cast<const ShieldCard*>(&card) != nullptr) {
        effect = CardEffect::ActivateShield;
        accent = DARKBLUE;
    } else if (dynamic_cast<const DiscountCard*>(&card) != nullptr) {
        effect = CardEffect::ActivateDiscount;
        accent = toolkit.theme().getGold();
    } else if (dynamic_cast<const LassoCard*>(&card) != nullptr ||
               dynamic_cast<const DemolitionCard*>(&card) != nullptr) {
        effect = CardEffect::ActivateShield;
        accent = toolkit.theme().getCoral();
    }

    return {card.getName(), card.getDescription(), kind, effect, magnitude, accent};
}

LogItem GUIGameController::makeLogItemFromBackend(const LogManager::LogEntry& entry) const {
    return {entry.getTurnNumber(), entry.getUsername(), entry.getActionType(), entry.getDetail()};
}

int GUIGameController::moveBackendPlayer(Player& player, int steps) {
    const int boardSize = backendGame_.getBoard().getBoardSize();
    if (boardSize <= 0) {
        player.moveTo(player.getPosition() + steps);
        return player.getPosition();
    }

    const int oldPosition = normalizedBackendTileIndex(player.getPosition());
    const int rawPosition = oldPosition + steps;
    const int newPosition = normalizedBackendTileIndex(rawPosition);
    if (rawPosition > boardSize) {
        int salary = backendGame_.getConfigManager().getGoSalary();
        std::shared_ptr<Tile> goTile = backendGame_.getBoard().getTile(1);
        GoTile* go = dynamic_cast<GoTile*>(goTile.get());
        if (go != nullptr) {
            salary = go->getSalary();
        }
        player += salary;
        addLog(player.getUsername(), "GO", "Melewati GO dan menerima M" + std::to_string(salary) + ".");
        addToast("Melewati GO: +" + std::to_string(salary) + ".", toolkit.theme().getGold());
    }

    player.moveTo(newPosition);
    return newPosition;
}

void GUIGameController::resolveBackendLanding(int backendTileIndex, bool fromMovement) {
    if (!backendGame_.isGameRunning() || backendGame_.getPlayers().empty()) {
        return;
    }

    const int normalizedIndex = normalizedBackendTileIndex(backendTileIndex);
    appState_.getGame().setSelectedTile(uiTileIndexFromBackend(normalizedIndex));
    Tile* tile = backendGame_.getBoard().getTile(normalizedIndex).get();
    if (tile == nullptr) {
        syncViewFromBackend();
        return;
    }

    Player& player = backendGame_.getCurrentPlayer();
    OverlayType overlayToOpen = OverlayType::None;
    int overlayTile = uiTileIndexFromBackend(normalizedIndex);

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
                    addToast("Mendarat di properti sendiri.", groupAccent(street.getColorGroup()));
                    break;
                }
                if (street.isMortgaged()) {
                    addToast("Properti tergadai, tidak menarik sewa.", groupAccent(street.getColorGroup()));
                    break;
                }
                RentContext rentContext = backendGame_.getPropertyManager().createRentContext(
                    backendGame_.getBoard(),
                    backendGame_.getConfigManager(),
                    backendGame_.getDice(),
                    street
                );
                const int rent = street.calculateRent(rentContext);
                if (player.isShieldActive()) {
                    player.setShieldActive(false);
                    addToast("Shield menahan sewa.", playerAccent(currentBackendPlayerIndex()));
                    break;
                }
                if (player.getMoney() < player.effectiveCost(rent)) {
                    backendGame_.getBankruptcyManager().beginBankruptcySession(player, owner, rent, false);
                    addLog(player.getUsername(), "SEWA", "Dana kurang untuk membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                    addToast("Dana kurang untuk membayar rent.", RED);
                    break;
                }
                player -= rent;
                *owner += player.effectiveCost(rent);
                addLog(player.getUsername(), "SEWA", "Membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                addToast("Bayar rent " + std::to_string(rent) + ".", groupAccent(street.getColorGroup()));
                break;
            }
            case Tile::TileType::Railroad:
            case Tile::TileType::Utility: {
                OwnableTile& property = static_cast<OwnableTile&>(*tile);
                Player* owner = property.getOwner();
                if (owner == nullptr || property.getOwnershipStatus() == OwnershipStatus::BANK) {
                    property.setOwner(&player);
                    property.setOwnershipStatus(OwnershipStatus::OWNED);
                    addLog(player.getUsername(), "AKUISISI", property.getName() + " menjadi milik pemain.");
                    addToast(property.getName() + " menjadi milikmu.", kindAccent(toGuiTileKind(property), ""));
                    break;
                }
                if (owner == &player) {
                    addToast("Mendarat di properti sendiri.", kindAccent(toGuiTileKind(property), ""));
                    break;
                }
                if (property.isMortgaged()) {
                    addToast("Properti tergadai, tidak menarik sewa.", kindAccent(toGuiTileKind(property), ""));
                    break;
                }
                RentContext rentContext = backendGame_.getPropertyManager().createRentContext(
                    backendGame_.getBoard(),
                    backendGame_.getConfigManager(),
                    backendGame_.getDice(),
                    property
                );
                const int rent = property.calculateRent(rentContext);
                if (player.isShieldActive()) {
                    player.setShieldActive(false);
                    addToast("Shield menahan sewa.", playerAccent(currentBackendPlayerIndex()));
                    break;
                }
                if (player.getMoney() < player.effectiveCost(rent)) {
                    backendGame_.getBankruptcyManager().beginBankruptcySession(player, owner, rent, false);
                    addLog(player.getUsername(), "SEWA", "Dana kurang untuk membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                    addToast("Dana kurang untuk membayar rent.", RED);
                    break;
                }
                player -= rent;
                *owner += player.effectiveCost(rent);
                addLog(player.getUsername(), "SEWA", "Membayar M" + std::to_string(rent) + " ke " + owner->getUsername() + ".");
                addToast("Bayar rent " + std::to_string(rent) + ".", kindAccent(toGuiTileKind(property), ""));
                break;
            }
            case Tile::TileType::Chance:
                syncViewFromBackend();
                openRandomCardDraw(kChanceDeckKey);
                return;
            case Tile::TileType::CommunityChest:
                syncViewFromBackend();
                openRandomCardDraw(kCommunityDeckKey);
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
                    addToast("Shield menahan efek jail.", playerAccent(currentBackendPlayerIndex()));
                    break;
                }
                backendGame_.getJailManager().sendToJail(player);
                player.moveTo(backendTileIndexFromUi(findJailIndex()));
                addLog(player.getUsername(), "JAIL", fromMovement ? "Efek kartu/perpindahan." : "Mendarat di Go To Jail.");
                overlayToOpen = OverlayType::Jail;
                overlayTile = findJailIndex();
                break;
            case Tile::TileType::Go:
                addToast("Mendarat di GO.", toolkit.theme().getGold());
                break;
            case Tile::TileType::Jail:
                addToast(player.isJailed() ? "Masih berada di jail." : "Hanya berkunjung ke jail.", toolkit.theme().getDanger());
                break;
            case Tile::TileType::FreeParking:
                addToast("Istirahat sejenak di Free Parking.", toolkit.theme().getPaper());
                break;
        }
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED);
    }

    syncViewFromBackend();
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
        maybeOpenLiquidation();
    }
}

void GUIGameController::closeCardDrawOverlay(bool discardPendingCard) {
    if (appState_.getOverlay().getType() == OverlayType::CardDraw) {
        clearPendingDrawnCard(discardPendingCard);
    }
}

void GUIGameController::clearPendingDrawnCard(bool discardPendingCard) {
    if (discardPendingCard && pendingChanceCard_ != nullptr) {
        backendGame_.getCardManager().discardChanceCard(pendingChanceCard_);
    }
    if (discardPendingCard && pendingCommunityChestCard_ != nullptr) {
        backendGame_.getCardManager().discardCommunityChestCard(pendingCommunityChestCard_);
    }
    pendingChanceCard_.reset();
    pendingCommunityChestCard_.reset();
}

void GUIGameController::discardAllCards(Player& player) {
    while (player.countCards() > 0) {
        backendGame_.getCardManager().dropHandCard(player, 0);
    }
}

void GUIGameController::configureSelectedHandCard(Player& player, int cardIndex) {
    std::vector<std::shared_ptr<HandCard>> cards = backendGame_.getCardManager().getHandCards(player);
    if (cardIndex < 0 || cardIndex >= static_cast<int>(cards.size()) || cards.at(cardIndex) == nullptr) {
        return;
    }

    if (std::shared_ptr<TeleportCard> teleport = std::dynamic_pointer_cast<TeleportCard>(cards.at(cardIndex))) {
        teleport->setTargetTileIndex(backendTileIndexFromUi(appState_.getGame().getSelectedTile()));
        return;
    }

    if (std::shared_ptr<LassoCard> lasso = std::dynamic_pointer_cast<LassoCard>(cards.at(cardIndex))) {
        for (Player& candidate : backendGame_.getPlayers()) {
            if (&candidate != &player && !candidate.isBankrupt()) {
                lasso->setTargetPlayer(&candidate);
                return;
            }
        }
    }

    if (std::shared_ptr<DemolitionCard> demolition = std::dynamic_pointer_cast<DemolitionCard>(cards.at(cardIndex))) {
        StreetTile* selectedStreet = streetFromUi(appState_.getGame().getSelectedTile());
        if (selectedStreet != nullptr &&
            selectedStreet->getOwner() != nullptr &&
            selectedStreet->getOwner() != &player &&
            selectedStreet->getBuildingLevel() > 0) {
            demolition->setTargetStreet(selectedStreet);
            return;
        }

        for (const std::shared_ptr<Tile>& tile : backendGame_.getBoard().getTiles()) {
            StreetTile* street = dynamic_cast<StreetTile*>(tile.get());
            if (street != nullptr &&
                street->getOwner() != nullptr &&
                street->getOwner() != &player &&
                street->getBuildingLevel() > 0) {
                demolition->setTargetStreet(street);
                return;
            }
        }
    }
}

std::vector<SaveSlot> GUIGameController::createInitialSaveSlots() const {
    return {
        {"save-1.txt", "File save lokal", 1, kMinPlayers, toolkit.theme().getCoral()},
        {"save-2.txt", "File save lokal", 1, kMinPlayers, toolkit.theme().getTeal()},
        {"save-3.txt", "File save lokal", 1, kMinPlayers, toolkit.theme().getGold()},
    };
}
