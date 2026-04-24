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
    : controllerContext_(
          appState_,
          backendGame_,
          guiTurnStarted_,
          diceRolledThisTurn_,
          pendingChanceCard_,
          pendingCommunityChestCard_,
          [this](const std::string& text, Color accent, float duration) { addToast(text, accent, duration); },
          [this](float deltaTime) { updateToasts(deltaTime); },
          [this](const std::string& actor, const std::string& action, const std::string& detail) { addLog(actor, action, detail); },
          [this]() { maybeOpenLiquidation(); },
          [this]() { syncViewFromBackend(); },
          [this](int backendIndex) { return uiTileIndexFromBackend(backendIndex); },
          [this](int uiIndex) { return backendTileIndexFromUi(uiIndex); },
          [this](int backendIndex) { return normalizedBackendTileIndex(backendIndex); },
          [this]() { return currentBackendPlayerIndex(); },
          [this](int uiIndex) { return ownableFromUi(uiIndex); },
          [this](int uiIndex) { return streetFromUi(uiIndex); },
          [this](const Tile& tile) { return toGuiTileKind(tile); },
          [this](const Card& card) { return makeCardInfoFromBackend(card); }
      ),
      cardController_(controllerContext_),
      commandController_(controllerContext_),
      tileController_(controllerContext_) {
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

void GUIGameController::tick(float deltaTime) { commandController_.tick(deltaTime); }

void GUIGameController::handleGlobalShortcuts() { commandController_.handleGlobalShortcuts(); }

void GUIGameController::openOverlay(OverlayType type) { commandController_.openOverlay(type); }

void GUIGameController::closeOverlay() { commandController_.closeOverlay(); }

bool GUIGameController::isOverlayOpen() const { return commandController_.isOverlayOpen(); }

int GUIGameController::getMortgageValue(const TileInfo& tile) const { return tileController_.getMortgageValue(tile); }

int GUIGameController::getRedeemCost(const TileInfo& tile) const { return tileController_.getRedeemCost(tile); }

int GUIGameController::findJailIndex() const { return tileController_.findJailIndex(); }

int GUIGameController::computeRent(const TileInfo& tile) const { return tileController_.computeRent(tile); }

int GUIGameController::computeTileAssetValue(const TileInfo& tile) const { return tileController_.computeTileAssetValue(tile); }

int GUIGameController::computeNetWorth(int playerIndex) const { return tileController_.computeNetWorth(playerIndex); }

int GUIGameController::jailFineAmount() const { return tileController_.jailFineAmount(); }

Rectangle GUIGameController::boardTileRect(Rectangle square, int index) const { return tileController_.boardTileRect(square, index); }

void GUIGameController::setSelectedTile(int tileIndex) { tileController_.setSelectedTile(tileIndex); }

void GUIGameController::openTileDetail(int tileIndex) { tileController_.openTileDetail(tileIndex); }

void GUIGameController::openPurchase(int tileIndex) { tileController_.openPurchase(tileIndex); }

void GUIGameController::openAuctionForTile(int tileIndex) { tileController_.openAuctionForTile(tileIndex); }

void GUIGameController::openIncomeTax() { tileController_.openIncomeTax(); }

void GUIGameController::openLuxuryTax() { tileController_.openLuxuryTax(); }

void GUIGameController::openFestival() { tileController_.openFestival(); }

void GUIGameController::openJail() { tileController_.openJail(); }

void GUIGameController::openCards() { cardController_.openCards(); }

void GUIGameController::openPortfolio() { commandController_.openPortfolio(); }

void GUIGameController::openBuild() { tileController_.openBuild(); }

void GUIGameController::openMortgage() { tileController_.openMortgage(); }

void GUIGameController::openRedeem() { tileController_.openRedeem(); }

void GUIGameController::openSave() { commandController_.openSave(); }

void GUIGameController::openLogs() { commandController_.openLogs(); }

void GUIGameController::openSetDice() { commandController_.openSetDice(); }

void GUIGameController::openHelp() { commandController_.openHelp(); }

void GUIGameController::openForceDrop() { commandController_.openForceDrop(); }

void GUIGameController::openLiquidation() { tileController_.openLiquidation(); }

void GUIGameController::openRandomCardDraw(int deckKey) { cardController_.openRandomCardDraw(deckKey); }

void GUIGameController::openGameOver() { commandController_.openGameOver(); }

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
        commandController_.startTurn();
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

void GUIGameController::startTurn() { commandController_.startTurn(); }

void GUIGameController::rollDice() { commandController_.rollDice(); }

void GUIGameController::applyManualDice() { commandController_.applyManualDice(); }

void GUIGameController::endTurn() { commandController_.endTurn(); }

void GUIGameController::triggerTileEvent(int tileIndex, bool fromMovement) { tileController_.triggerTileEvent(tileIndex, fromMovement); }

bool GUIGameController::canSaveNow() const { return commandController_.canSaveNow(); }

void GUIGameController::adjustManualDie(int dieIndex, int delta) { commandController_.adjustManualDie(dieIndex, delta); }

bool GUIGameController::canCurrentPlayerAffordSelectedPurchase() const { return tileController_.canCurrentPlayerAffordSelectedPurchase(); }

int GUIGameController::currentPurchasePrice() const { return tileController_.currentPurchasePrice(); }

void GUIGameController::buySelectedProperty() { tileController_.buySelectedProperty(); }

void GUIGameController::skipSelectedPurchase() { tileController_.skipSelectedPurchase(); }

void GUIGameController::auctionRaiseBid(int amount) { tileController_.auctionRaiseBid(amount); }

void GUIGameController::auctionPass() { tileController_.auctionPass(); }

void GUIGameController::finalizeAuction() { tileController_.finalizeAuction(); }

int GUIGameController::flatIncomeTax() const { return tileController_.flatIncomeTax(); }

int GUIGameController::percentageIncomeTax() const { return tileController_.percentageIncomeTax(); }

void GUIGameController::payIncomeTax(bool useFlatTax) { tileController_.payIncomeTax(useFlatTax); }

int GUIGameController::luxuryTaxAmount() const { return tileController_.luxuryTaxAmount(); }

void GUIGameController::payLuxuryTax() { tileController_.payLuxuryTax(); }

void GUIGameController::activateFestivalOnSelectedTile() { tileController_.activateFestivalOnSelectedTile(); }

void GUIGameController::payJailFine() { tileController_.payJailFine(); }

void GUIGameController::attemptJailRoll() { tileController_.attemptJailRoll(); }

void GUIGameController::useJailCard() { cardController_.useJailCard(); }

void GUIGameController::useSelectedHandCard() { cardController_.useSelectedHandCard(); }

void GUIGameController::storeDrawnCard() { cardController_.storeDrawnCard(); }

void GUIGameController::applyDrawnCard() { cardController_.applyDrawnCard(); }

void GUIGameController::buildOnSelectedTile() { tileController_.buildOnSelectedTile(); }

void GUIGameController::mortgageSelectedTile() { tileController_.mortgageSelectedTile(); }

void GUIGameController::redeemSelectedTile() { tileController_.redeemSelectedTile(); }

void GUIGameController::saveSession() { commandController_.saveSession(); }

void GUIGameController::dropSelectedHandCard() { cardController_.dropSelectedHandCard(); }

void GUIGameController::liquidateSelectedTile() { tileController_.liquidateSelectedTile(); }

void GUIGameController::declareBankrupt() { tileController_.declareBankrupt(); }

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

std::vector<int> GUIGameController::currentPlayerStreetOptions() const { return tileController_.currentPlayerStreetOptions(); }

std::vector<int> GUIGameController::currentPlayerBuildOptions() const { return tileController_.currentPlayerBuildOptions(); }

std::vector<int> GUIGameController::currentPlayerMortgageOptions() const { return tileController_.currentPlayerMortgageOptions(); }

std::vector<int> GUIGameController::currentPlayerRedeemOptions() const { return tileController_.currentPlayerRedeemOptions(); }

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

std::vector<SaveSlot> GUIGameController::createInitialSaveSlots() const {
    return {
        {"save-1.txt", "File save lokal", 1, kMinPlayers, toolkit.theme().getCoral()},
        {"save-2.txt", "File save lokal", 1, kMinPlayers, toolkit.theme().getTeal()},
        {"save-3.txt", "File save lokal", 1, kMinPlayers, toolkit.theme().getGold()},
    };
}
