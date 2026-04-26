#include "controller/GUIController/GUIGameController.hpp"

#include "model/ComputerDecisionMaker.hpp"
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
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace view::raylibgui;

Color GUIGameController::playerAccent(int index) {
    UiToolkit toolkit;
    switch (index) {
        case 0: return toolkit.theme().getCoral();
        case 1: return toolkit.theme().getTeal();
        case 2: return toolkit.theme().getGold();
        default: return toolkit.theme().getNavy();
    }
}

std::string GUIGameController::normalizeKey(std::string value) {
    std::string normalized;
    for (char ch : value) {
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            normalized += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
    }
    return normalized;
}

Color GUIGameController::groupAccent(const std::string& group) {
    UiToolkit toolkit;
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

Color GUIGameController::kindAccent(TileKind kind, const std::string& group) {
    UiToolkit toolkit;
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

std::string GUIGameController::tileFlavor(TileKind kind) {
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

bool GUIGameController::hasTxtExtension(const std::string& filename) {
    if (filename.size() < 4) {
        return false;
    }
    std::string suffix = filename.substr(filename.size() - 4);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return suffix == ".txt";
}

std::string GUIGameController::withTxtExtension(std::string filename) {
    return hasTxtExtension(filename) ? filename : filename + ".txt";
}

std::string GUIGameController::trimWhitespace(const std::string& value) {
    const std::string whitespace = " \t\n\r\f\v";
    const std::size_t first = value.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return "";
    }

    const std::size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
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
    refreshDefaultConfigPreview();
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
    commandController_.tick(deltaTime);

    if (computerActionCooldown_ > 0.0f) {
        computerActionCooldown_ = std::max(0.0f, computerActionCooldown_ - deltaTime);
    }

    if (computerActionCooldown_ <= 0.0f && driveComputerTurn()) {
        computerActionCooldown_ = 0.45f;
    }
}

void GUIGameController::handleGlobalShortcuts() { commandController_.handleGlobalShortcuts(); }

void GUIGameController::openOverlay(OverlayType type) { commandController_.openOverlay(type); }

void GUIGameController::closeOverlay() { commandController_.closeOverlay(); }

bool GUIGameController::isOverlayOpen() const { return commandController_.isOverlayOpen(); }

int GUIGameController::getMortgageValue(const TileInfo& tile) const { return tileController_.getMortgageValue(tile); }

int GUIGameController::getRedeemCost(const TileInfo& tile) const { return tileController_.getRedeemCost(tile); }

int GUIGameController::getSellToBankValue(const TileInfo& tile) const { return tileController_.getSellToBankValue(tile); }

TileDeedInfo GUIGameController::deedInfoForTile(const TileInfo& tile) const { return tileController_.deedInfoForTile(tile); }

int GUIGameController::findJailIndex() const { return tileController_.findJailIndex(); }

int GUIGameController::computeRent(const TileInfo& tile) const { return tileController_.computeRent(tile); }

int GUIGameController::computeTileAssetValue(const TileInfo& tile) const { return tileController_.computeTileAssetValue(tile); }

int GUIGameController::computeNetWorth(int playerIndex) const { return tileController_.computeNetWorth(playerIndex); }

int GUIGameController::jailFineAmount() const { return tileController_.jailFineAmount(); }

int GUIGameController::gridCellCount() const { return tileController_.gridCellCount(); }

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

int GUIGameController::defaultConfigStartingCash() const { return defaultConfigStartingCash_; }

int GUIGameController::defaultConfigTurnLimit() const { return defaultConfigTurnLimit_; }

void GUIGameController::refreshDefaultConfigPreview() {
    try {
        backendGame_.getConfigManager().loadAllConfigs();
        const int configStartingCash = backendGame_.getConfigManager().getInitialBalance();
        const int configTurnLimit = backendGame_.getConfigManager().getMaxTurn();
        defaultConfigStartingCash_ = configStartingCash > 0 ? configStartingCash : 1500;
        defaultConfigTurnLimit_ = configTurnLimit;
    } catch (const std::exception& exception) {
        defaultConfigStartingCash_ = 1500;
        defaultConfigTurnLimit_ = 50;
        addToast(exception.what(), RED, 6.0f);
    }
}

void GUIGameController::startFreshSession() {
    try {
        closeOverlay();
        backendGame_ = Game();
        backendGame_.getConfigManager().loadAllConfigs();
        const int configStartingCash = backendGame_.getConfigManager().getInitialBalance();
        const int configTurnLimit = backendGame_.getConfigManager().getMaxTurn();
        defaultConfigStartingCash_ = configStartingCash > 0 ? configStartingCash : 1500;
        defaultConfigTurnLimit_ = configTurnLimit;

        std::vector<Player>& players = backendGame_.getPlayers();
        players.clear();
        const int playerCount = std::max(kMinPlayers, std::min(kMaxPlayers, appState_.getPlayerCount()));
        const int computerPlayerCount = std::max(0, std::min(appState_.getComputerPlayerCount(), playerCount));
        const int humanPlayerCount = playerCount - computerPlayerCount;
        appState_.setComputerPlayerCount(computerPlayerCount);
        const bool useDefaultConfig = appState_.getNewGameConfigMode() == NewGameConfigMode::DefaultConfig;
        const int startingCash = useDefaultConfig
            ? defaultConfigStartingCash_
            : (appState_.getStartingCash() > 0 ? appState_.getStartingCash() : defaultConfigStartingCash_);

        for (int index = 0; index < humanPlayerCount; index++) {
            std::string name = appState_.getPlayerNames().at(index);
            if (name.empty()) {
                name = "Pemain" + std::to_string(index + 1);
            }
            players.push_back(Player(
                name,
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
        for (int index = 0; index < computerPlayerCount; index++) {
            const std::string name = index == 0
                ? "ProfessorRayapSunggal"
                : "ProfessorRayapSunggal" + std::to_string(index + 1);
            Player computer(
                name,
                startingCash,
                1,
                PlayerStatus::ACTIVE,
                0,
                false,
                false,
                0,
                0
            );
            computer.setComputerPlayer(true);
            players.push_back(computer);
        }
        std::shuffle(players.begin(), players.end(), appState_.getRng());

        backendGame_.startNewGame();
        if (!useDefaultConfig && appState_.getTurnLimit() > 0) {
            backendGame_.getGameContext().setMaxTurn(appState_.getTurnLimit());
        }

        guiTurnStarted_ = false;
        diceRolledThisTurn_ = false;
        computerActionCooldown_ = 0.25f;
        appState_.setScreen(Screen::Gameplay);
        syncViewFromBackend();
        commandController_.startTurn();
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED, 6.0f);
    }
}

void GUIGameController::loadSessionFromInput() {
    std::string filename = trimWhitespace(appState_.getLoadInput());
    if (filename.empty()) {
        addToast("Masukkan nama file save terlebih dahulu.", RED, 4.0f);
        return;
    }
    filename = withTxtExtension(filename);

    try {
        closeOverlay();
        backendGame_ = Game();
        backendGame_.loadGame(filename);
        guiTurnStarted_ = true;
        diceRolledThisTurn_ = false;
        computerActionCooldown_ = 0.25f;
        appState_.setScreen(Screen::Gameplay);
        syncViewFromBackend();
        addToast("Permainan dimuat dari " + filename + ".", SKYBLUE);
    } catch (const std::exception& exception) {
        addToast(exception.what(), RED, 6.0f);
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

bool GUIGameController::canActiveAuctionBidderPass() const { return tileController_.canActiveAuctionBidderPass(); }

int GUIGameController::currentPurchasePrice() const { return tileController_.currentPurchasePrice(); }

void GUIGameController::buySelectedProperty() { tileController_.buySelectedProperty(); }

void GUIGameController::skipSelectedPurchase() { tileController_.skipSelectedPurchase(); }

void GUIGameController::auctionPlaceBid(int targetBid) { tileController_.auctionPlaceBid(targetBid); }

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

void GUIGameController::mortgageLiquidationSelectedTile() { tileController_.mortgageLiquidationSelectedTile(); }

void GUIGameController::declareBankrupt() { tileController_.declareBankrupt(); }

bool GUIGameController::canUseHandCardNow() const {
    if (!backendGame_.isGameRunning() ||
        backendGame_.getPlayers().empty() ||
        !guiTurnStarted_) {
        return false;
    }

    const Player& player = backendGame_.getCurrentPlayer();
    return !player.hasUsedHandCardThisTurn() &&
           player.countCards() > 0 &&
           !backendGame_.getTurnManager().hasDiceRolledOnceThisTurn();
}

bool GUIGameController::isLiquidationRequired() const {
    if (!backendGame_.isGameRunning() || backendGame_.getPlayers().empty()) {
        return false;
    }
    return backendGame_.getBankruptcyManager().isBankruptcyActive() ||
           backendGame_.getCurrentPlayer().getMoney() < 0;
}

std::vector<int> GUIGameController::currentTeleportCardTargets() const {
    return cardController_.teleportTargetOptions();
}

std::vector<int> GUIGameController::currentDemolitionCardTargets() const {
    return cardController_.demolitionTargetOptions();
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
    for (int index = 0; index < static_cast<int>(backendLogs.size()); index++) {
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

std::vector<int> GUIGameController::currentPlayerOwnableOptions() const { return tileController_.currentPlayerOwnableOptions(); }

std::vector<int> GUIGameController::currentPlayerBuildOptions() const { return tileController_.currentPlayerBuildOptions(); }

std::vector<int> GUIGameController::currentPlayerMortgageOptions() const { return tileController_.currentPlayerMortgageOptions(); }

std::vector<int> GUIGameController::currentPlayerRedeemOptions() const { return tileController_.currentPlayerRedeemOptions(); }

bool GUIGameController::driveComputerTurn() {
    if (appState_.getScreen() != Screen::Gameplay ||
        !backendGame_.isGameRunning() ||
        backendGame_.getPlayers().empty()) {
        return false;
    }

    if (appState_.getOverlay().getType() != OverlayType::None) {
        return handleComputerOverlay();
    }

    return handleComputerFreeTurn();
}

bool GUIGameController::handleComputerOverlay() {
    OverlayState& overlay = appState_.getOverlay();
    const OverlayType type = overlay.getType();
    const int currentIndex = currentBackendPlayerIndex();
    const bool currentComputer = isBackendPlayerComputer(currentIndex);

    switch (type) {
        case OverlayType::Purchase:
            if (!currentComputer) {
                return false;
            }
            if (ComputerDecisionMaker::decideToBuy() && canCurrentPlayerAffordSelectedPurchase()) {
                buySelectedProperty();
            } else {
                skipSelectedPurchase();
            }
            return true;

        case OverlayType::Auction: {
            AuctionState& auction = overlay.getAuction();
            const int bidderIndex = auction.getSelectedBidder();
            if (!isBackendPlayerComputer(bidderIndex)) {
                return false;
            }

            Player& bidder = backendGame_.getPlayers().at(bidderIndex);
            const int minimumBid = auction.getHighestBidder() < 0 ? 0 : auction.getHighestBid() + 1;
            const bool forcedBid = !canActiveAuctionBidderPass();
            const std::string decision = ComputerDecisionMaker::decideAuctionAction(minimumBid, bidder.getMoney(), forcedBid);
            if (decision.rfind("BID ", 0) == 0) {
                std::istringstream stream(decision);
                std::string ignored;
                int bid = 0;
                stream >> ignored >> bid;
                if (bid >= minimumBid && bidder.getMoney() >= bidder.effectiveCost(bid)) {
                    auctionPlaceBid(bid);
                } else {
                    auctionPass();
                }
            } else {
                auctionPass();
            }
            return true;
        }

        case OverlayType::IncomeTax:
            if (!currentComputer) {
                return false;
            }
            payIncomeTax(ComputerDecisionMaker::decideIncomeTax() == 1);
            return true;

        case OverlayType::LuxuryTax:
            if (!currentComputer) {
                return false;
            }
            payLuxuryTax();
            return true;

        case OverlayType::Festival: {
            if (!currentComputer) {
                return false;
            }
            const std::vector<int> options = currentPlayerOwnableOptions();
            const int choice = ComputerDecisionMaker::decideFestival(static_cast<int>(options.size()));
            if (options.empty() || choice <= 0) {
                closeOverlay();
                commandController_.finishTurnAfterDiceIfReady();
            } else {
                overlay.setSelectedIndex(std::max(0, std::min(choice - 1, static_cast<int>(options.size()) - 1)));
                activateFestivalOnSelectedTile();
            }
            return true;
        }

        case OverlayType::Jail:
            if (!currentComputer) {
                return false;
            }
            if (!backendGame_.getCurrentPlayer().isJailed()) {
                closeOverlay();
            } else if (backendGame_.getCurrentPlayer().getFailedJailRolls() >= 3 ||
                       ComputerDecisionMaker::decideJailChoice() == 1) {
                payJailFine();
            } else {
                attemptJailRoll();
            }
            return true;

        case OverlayType::ForceDrop: {
            if (!currentComputer) {
                return false;
            }
            Player& player = backendGame_.getCurrentPlayer();
            const int cardCount = player.countCards();
            if (cardCount <= 3) {
                closeOverlay();
            } else {
                const int choice = ComputerDecisionMaker::decideForceDropChoice(cardCount);
                overlay.setSelectedIndex(std::max(0, std::min(choice - 1, cardCount - 1)));
                dropSelectedHandCard();
            }
            return true;
        }

        case OverlayType::Liquidation: {
            const int debtorIndex = activeLiquidationPlayerIndex();
            if (!isBackendPlayerComputer(debtorIndex)) {
                return false;
            }
            if (!isLiquidationRequired()) {
                closeOverlay();
                return true;
            }

            Player& debtor = backendGame_.getPlayers().at(debtorIndex);
            std::vector<OwnableTile*> properties = backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), debtor);
            std::vector<std::pair<int, bool>> actions;
            for (int index = 0; index < static_cast<int>(properties.size()); index++) {
                OwnableTile* property = properties.at(index);
                if (property == nullptr) {
                    continue;
                }
                actions.push_back({index, false});
                if (backendGame_.getPropertyManager().canMortgage(debtor, *property)) {
                    actions.push_back({index, true});
                }
            }

            if (actions.empty()) {
                declareBankrupt();
                return true;
            }

            const int choice = ComputerDecisionMaker::decideLiquidation(static_cast<int>(actions.size()));
            if (choice <= 0) {
                declareBankrupt();
                return true;
            }

            const std::pair<int, bool>& action = actions.at(std::max(0, std::min(choice - 1, static_cast<int>(actions.size()) - 1)));
            overlay.setSelectedIndex(action.first);
            if (action.second) {
                mortgageLiquidationSelectedTile();
            } else {
                liquidateSelectedTile();
            }
            return true;
        }

        case OverlayType::CardDraw:
            if (!currentComputer) {
                return false;
            }
            if (overlay.getCard().getKind() == CardKind::Hand) {
                storeDrawnCard();
            } else {
                applyDrawnCard();
            }
            return true;

        default:
            return false;
    }
}

bool GUIGameController::handleComputerFreeTurn() {
    const int currentIndex = currentBackendPlayerIndex();
    if (!isBackendPlayerComputer(currentIndex) || !guiTurnStarted_) {
        return false;
    }

    Player& player = backendGame_.getCurrentPlayer();
    if (player.isBankrupt()) {
        endTurn();
        return true;
    }

    if (backendGame_.getBankruptcyManager().isBankruptcyActive() || player.getMoney() < 0) {
        maybeOpenLiquidation();
        return true;
    }

    if (backendGame_.getCardManager().needsForceDrop(player)) {
        openForceDrop();
        return true;
    }

    if (player.isJailed()) {
        openJail();
        return true;
    }

    if (backendGame_.getTurnManager().isRolledThisTurn()) {
        commandController_.finishTurnAfterDiceIfReady();
        return true;
    }

    if (!player.hasUsedHandCardThisTurn() &&
        player.countCards() > 0 &&
        !backendGame_.getTurnManager().hasDiceRolledOnceThisTurn() &&
        ComputerDecisionMaker::rollThreshold(10)) {
        std::vector<int> usableCards;
        for (int index = 0; index < player.countCards(); index++) {
            if (canComputerUseCard(player, index)) {
                usableCards.push_back(index);
            }
        }

        if (!usableCards.empty()) {
            const int choice = ComputerDecisionMaker::decideAbilityCard(static_cast<int>(usableCards.size()));
            const int cardIndex = usableCards.at(std::max(0, std::min(choice - 1, static_cast<int>(usableCards.size()) - 1)));
            appState_.getOverlay().setSelectedIndex(cardIndex);
            if (prepareComputerCard(player, cardIndex)) {
                useSelectedHandCard();
                return true;
            }
        }
    }

    const std::vector<int> buildOptions = currentPlayerBuildOptions();
    if (!buildOptions.empty() && ComputerDecisionMaker::rollThreshold(60)) {
        const int choice = ComputerDecisionMaker::decideBuildTile(static_cast<int>(buildOptions.size()));
        appState_.getOverlay().setSelectedIndex(std::max(0, std::min(choice - 1, static_cast<int>(buildOptions.size()) - 1)));
        buildOnSelectedTile();
        return true;
    }

    rollDice();
    return true;
}

bool GUIGameController::canComputerUseCard(const Player& player, int cardIndex) const {
    const std::vector<std::shared_ptr<HandCard>> cards = backendGame_.getCardManager().getHandCards(player);
    if (cardIndex < 0 || cardIndex >= static_cast<int>(cards.size()) || cards.at(cardIndex) == nullptr) {
        return false;
    }

    if (std::dynamic_pointer_cast<TeleportCard>(cards.at(cardIndex)) != nullptr) {
        return backendGame_.getBoard().getBoardSize() > 0;
    }

    if (std::dynamic_pointer_cast<LassoCard>(cards.at(cardIndex)) != nullptr) {
        const int myPosition = player.getPosition();
        const int boardSize = backendGame_.getBoard().getBoardSize();
        for (const Player& candidate : backendGame_.getPlayers()) {
            if (&candidate == &player || candidate.isBankrupt() || candidate.isJailed()) {
                continue;
            }
            const int otherPosition = candidate.getPosition();
            const bool isAhead = (otherPosition > myPosition) ||
                                 (otherPosition < myPosition && boardSize > 0 && (myPosition - otherPosition) > boardSize / 2);
            if (isAhead) {
                return true;
            }
        }
        return false;
    }

    if (std::dynamic_pointer_cast<DemolitionCard>(cards.at(cardIndex)) != nullptr) {
        for (const Player& opponent : backendGame_.getPlayers()) {
            if (&opponent == &player || opponent.isBankrupt()) {
                continue;
            }
            for (OwnableTile* property : backendGame_.getPropertyManager().getOwnedProperties(backendGame_.getBoard(), opponent)) {
                StreetTile* street = dynamic_cast<StreetTile*>(property);
                if (street != nullptr && street->getBuildingLevel() > 0) {
                    return true;
                }
            }
        }
        return false;
    }

    return true;
}

bool GUIGameController::prepareComputerCard(Player& player, int cardIndex) {
    const std::vector<std::shared_ptr<HandCard>> cards = backendGame_.getCardManager().getHandCards(player);
    if (cardIndex < 0 || cardIndex >= static_cast<int>(cards.size()) || cards.at(cardIndex) == nullptr) {
        return false;
    }

    if (std::dynamic_pointer_cast<TeleportCard>(cards.at(cardIndex)) != nullptr) {
        const int boardSize = backendGame_.getBoard().getBoardSize();
        if (boardSize <= 0) {
            return false;
        }
        appState_.getGame().setSelectedTile(ComputerDecisionMaker::randomInt(0, boardSize - 1));
    }

    return canComputerUseCard(player, cardIndex);
}

bool GUIGameController::isBackendPlayerComputer(int playerIndex) const {
    const std::vector<Player>& players = backendGame_.getPlayers();
    return playerIndex >= 0 &&
           playerIndex < static_cast<int>(players.size()) &&
           players.at(playerIndex).isComputerPlayer();
}

int GUIGameController::activeLiquidationPlayerIndex() const {
    Player* debtor = backendGame_.getBankruptcyManager().getPendingDebtor();
    if (debtor != nullptr) {
        const int debtorIndex = backendPlayerIndex(debtor);
        if (debtorIndex >= 0) {
            return debtorIndex;
        }
    }

    const int selectedPlayer = appState_.getOverlay().getSelectedPlayer();
    if (selectedPlayer >= 0 && selectedPlayer < static_cast<int>(backendGame_.getPlayers().size())) {
        return selectedPlayer;
    }

    return currentBackendPlayerIndex();
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
        const int turn = backendGame_.getCurrentTurn();
        backendGame_.getLogManager().addLog(turn, actor, action, detail);
        GameState& game = appState_.getGame();
        game.getLogs().push_back({turn, actor, action, detail});
        return;
    }

    GameState& game = appState_.getGame();
    game.getLogs().push_back({game.getTurn(), actor, action, detail});
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
        if (dynamic_cast<const UtilityTile*>(ownable) != nullptr) {
            RentContext rentContext = backendGame_.getPropertyManager().createRentContext(
                backendGame_.getBoard(),
                backendGame_.getConfigManager(),
                backendGame_.getDice(),
                *ownable
            );
            baseRent = rentContext.getUtilityMultiplier();
            if (baseRent <= 0) {
                baseRent = backendGame_.getConfigManager().getUtilityMultiplier(1);
            }
        } else {
            RentContext rentContext = backendGame_.getPropertyManager().createRentContext(
                backendGame_.getBoard(),
                backendGame_.getConfigManager(),
                backendGame_.getDice(),
                *ownable
            );
            baseRent = ownable->calculateRent(rentContext);
        }
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
        cards,
        player.isComputerPlayer()
    };
}

CardInfo GUIGameController::makeCardInfoFromBackend(const Card& card) const {
    UiToolkit toolkit;
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
    } else if (dynamic_cast<const DemolitionCard*>(&card) != nullptr) {
        effect = CardEffect::DemolishBuilding;
        accent = toolkit.theme().getCoral();
    } else if (dynamic_cast<const LassoCard*>(&card) != nullptr) {
        effect = CardEffect::ActivateShield;
        accent = toolkit.theme().getCoral();
    }

    return {card.getName(), card.getDescription(), kind, effect, magnitude, accent};
}

LogItem GUIGameController::makeLogItemFromBackend(const LogManager::LogEntry& entry) const {
    return {entry.getTurnNumber(), entry.getUsername(), entry.getActionType(), entry.getDetail()};
}

