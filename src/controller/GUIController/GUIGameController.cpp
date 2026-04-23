#include "controller/GUIController/GUIGameController.hpp"

#include "view/raylib/UiToolkit.hpp"

#include <algorithm>
#include <ctime>

using namespace view::raylibgui;

namespace {
UiToolkit toolkit;

Color playerAccent(int index) {
    switch (index) {
        case 0: return toolkit.theme().coral;
        case 1: return toolkit.theme().teal;
        case 2: return toolkit.theme().gold;
        default: return toolkit.theme().navy;
    }
}
}

GUIGameController::GUIGameController()
    : cardController_(*this),
      commandController_(*this),
      tileController_(*this) {
    appState_.rng = std::mt19937(static_cast<unsigned int>(std::time(nullptr)));
    appState_.saveSlots = createInitialSaveSlots();
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
    updateToasts(deltaTime);
}

void GUIGameController::handleGlobalShortcuts() {
    commandController_.handleGlobalShortcuts();
}

void GUIGameController::openOverlay(OverlayType type) {
    commandController_.openOverlay(type);
}

void GUIGameController::closeOverlay() {
    commandController_.closeOverlay();
}

bool GUIGameController::isOverlayOpen() const {
    return commandController_.isOverlayOpen();
}

int GUIGameController::getMortgageValue(const TileInfo& tile) const {
    return tileController_.getMortgageValue(tile);
}

int GUIGameController::getRedeemCost(const TileInfo& tile) const {
    return tileController_.getRedeemCost(tile);
}

int GUIGameController::findJailIndex() const {
    return tileController_.findJailIndex();
}

int GUIGameController::computeRent(const TileInfo& tile) const {
    return tileController_.computeRent(tile);
}

int GUIGameController::computeTileAssetValue(const TileInfo& tile) const {
    return tileController_.computeTileAssetValue(tile);
}

int GUIGameController::computeNetWorth(int playerIndex) const {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(appState_.game.players.size())) {
        return 0;
    }

    const PlayerInfo& player = appState_.game.players.at(playerIndex);
    int wealth = player.money;
    for (int tileIndex : player.properties) {
        wealth += computeTileAssetValue(appState_.game.board.at(tileIndex));
    }
    wealth += static_cast<int>(player.handCards.size()) * 75;
    return wealth;
}

Rectangle GUIGameController::boardTileRect(Rectangle square, int index) const {
    return tileController_.boardTileRect(square, index);
}

void GUIGameController::setSelectedTile(int tileIndex) {
    tileController_.setSelectedTile(tileIndex);
}

void GUIGameController::openTileDetail(int tileIndex) {
    tileController_.openTileDetail(tileIndex);
}

void GUIGameController::openPurchase(int tileIndex) {
    tileController_.openPurchase(tileIndex);
}

void GUIGameController::openAuctionForTile(int tileIndex) {
    tileController_.openAuctionForTile(tileIndex);
}

void GUIGameController::openIncomeTax() {
    tileController_.openIncomeTax();
}

void GUIGameController::openLuxuryTax() {
    tileController_.openLuxuryTax();
}

void GUIGameController::openFestival() {
    tileController_.openFestival();
}

void GUIGameController::openJail() {
    tileController_.openJail();
}

void GUIGameController::openCards() {
    cardController_.openCards();
}

void GUIGameController::openPortfolio() {
    commandController_.openPortfolio();
}

void GUIGameController::openBuild() {
    tileController_.openBuild();
}

void GUIGameController::openMortgage() {
    tileController_.openMortgage();
}

void GUIGameController::openRedeem() {
    tileController_.openRedeem();
}

void GUIGameController::openSave() {
    commandController_.openSave();
}

void GUIGameController::openLogs() {
    commandController_.openLogs();
}

void GUIGameController::openSetDice() {
    commandController_.openSetDice();
}

void GUIGameController::openHelp() {
    commandController_.openHelp();
}

void GUIGameController::openForceDrop() {
    commandController_.openForceDrop();
}

void GUIGameController::openLiquidation() {
    tileController_.openLiquidation();
}

void GUIGameController::openRandomCardDraw(int deckKey) {
    cardController_.openRandomCardDraw(deckKey);
}

void GUIGameController::openGameOver() {
    commandController_.openGameOver();
}

void GUIGameController::startFreshSession() {
    closeOverlay();
    appState_.game = createBaseGame();
    appState_.screen = Screen::Gameplay;
}

void GUIGameController::loadSessionFromSlot(int slotIndex) {
    appState_.selectedSave = slotIndex;
    appState_.game = createBaseGame();
    applyScenario(appState_.game, slotIndex);
    appState_.screen = Screen::Gameplay;
    closeOverlay();
}

void GUIGameController::startTurn() {
    commandController_.startTurn();
}

void GUIGameController::rollDice() {
    commandController_.rollDice();
}

void GUIGameController::applyManualDice() {
    commandController_.applyManualDice();
}

void GUIGameController::endTurn() {
    commandController_.endTurn();
}

void GUIGameController::triggerTileEvent(int tileIndex, bool fromMovement) {
    tileController_.triggerTileEvent(tileIndex, fromMovement);
}

bool GUIGameController::canSaveNow() const {
    return commandController_.canSaveNow();
}

void GUIGameController::adjustManualDie(int dieIndex, int delta) {
    commandController_.adjustManualDie(dieIndex, delta);
}

bool GUIGameController::canCurrentPlayerAffordSelectedPurchase() const {
    return tileController_.canCurrentPlayerAffordSelectedPurchase();
}

int GUIGameController::currentPurchasePrice() const {
    return tileController_.currentPurchasePrice();
}

void GUIGameController::buySelectedProperty() {
    tileController_.buySelectedProperty();
}

void GUIGameController::skipSelectedPurchase() {
    tileController_.skipSelectedPurchase();
}

void GUIGameController::auctionRaiseBid(int amount) {
    tileController_.auctionRaiseBid(amount);
}

void GUIGameController::auctionPass() {
    tileController_.auctionPass();
}

void GUIGameController::finalizeAuction() {
    tileController_.finalizeAuction();
}

int GUIGameController::flatIncomeTax() const {
    return tileController_.flatIncomeTax();
}

int GUIGameController::percentageIncomeTax() const {
    return tileController_.percentageIncomeTax();
}

void GUIGameController::payIncomeTax(bool useFlatTax) {
    tileController_.payIncomeTax(useFlatTax);
}

int GUIGameController::luxuryTaxAmount() const {
    return tileController_.luxuryTaxAmount();
}

void GUIGameController::payLuxuryTax() {
    tileController_.payLuxuryTax();
}

void GUIGameController::activateFestivalOnSelectedTile() {
    tileController_.activateFestivalOnSelectedTile();
}

void GUIGameController::payJailFine() {
    tileController_.payJailFine();
}

void GUIGameController::useJailCard() {
    cardController_.useJailCard();
}

void GUIGameController::useSelectedHandCard() {
    cardController_.useSelectedHandCard();
}

void GUIGameController::storeDrawnCard() {
    cardController_.storeDrawnCard();
}

void GUIGameController::applyDrawnCard() {
    cardController_.applyDrawnCard();
}

void GUIGameController::buildOnSelectedTile() {
    tileController_.buildOnSelectedTile();
}

void GUIGameController::mortgageSelectedTile() {
    tileController_.mortgageSelectedTile();
}

void GUIGameController::redeemSelectedTile() {
    tileController_.redeemSelectedTile();
}

void GUIGameController::saveSession() {
    commandController_.saveSession();
}

void GUIGameController::dropSelectedHandCard() {
    cardController_.dropSelectedHandCard();
}

void GUIGameController::liquidateSelectedTile() {
    tileController_.liquidateSelectedTile();
}

void GUIGameController::declareBankrupt() {
    tileController_.declareBankrupt();
}

std::vector<int> GUIGameController::currentPlayerStreetOptions() const {
    return tileController_.currentPlayerStreetOptions();
}

std::vector<int> GUIGameController::currentPlayerBuildOptions() const {
    return tileController_.currentPlayerBuildOptions();
}

std::vector<int> GUIGameController::currentPlayerMortgageOptions() const {
    return tileController_.currentPlayerMortgageOptions();
}

std::vector<int> GUIGameController::currentPlayerRedeemOptions() const {
    return tileController_.currentPlayerRedeemOptions();
}

void GUIGameController::addToast(const std::string& text, Color accent, float duration) {
    if (appState_.game.toasts.size() >= 4) {
        appState_.game.toasts.pop_front();
    }

    appState_.game.toasts.push_back({text, accent, duration, duration});
}

void GUIGameController::updateToasts(float deltaTime) {
    for (Toast& toast : appState_.game.toasts) {
        toast.timeLeft -= deltaTime;
    }

    while (!appState_.game.toasts.empty() && appState_.game.toasts.front().timeLeft <= 0.0f) {
        appState_.game.toasts.pop_front();
    }
}

void GUIGameController::addLog(const std::string& actor, const std::string& action, const std::string& detail) {
    appState_.game.logs.push_back({appState_.game.turn, actor, action, detail});
    if (static_cast<int>(appState_.game.logs.size()) > kMaxLogEntries) {
        appState_.game.logs.erase(appState_.game.logs.begin());
    }
}

void GUIGameController::maybeOpenLiquidation() {
    if (appState_.game.players.empty()) {
        return;
    }

    if (appState_.game.players.at(appState_.game.currentPlayer).money < 0) {
        openLiquidation();
    }
}

GameState GUIGameController::createBaseGame() const {
    GameState game;
    game.sessionLabel = "NIMONSPOLI";
    game.board = createBoard();
    game.turn = 1;
    game.turnLimit = std::max(12, appState_.turnLimit);
    game.currentPlayer = 0;
    game.selectedTile = 0;
    game.startCash = appState_.startingCash;

    for (int index = 0; index < appState_.playerCount; index++) {
        PlayerInfo player;
        player.name = appState_.playerNames.at(index);
        player.accent = playerAccent(index);
        player.money = appState_.startingCash;
        player.position = 0;
        game.players.push_back(player);
    }

    return game;
}

void GUIGameController::applyScenario(GameState& game, int scenarioIndex) const {
    if (game.players.empty()) {
        return;
    }

    if (scenarioIndex == 0) {
        game.turn = 7;
    } else if (scenarioIndex == 1) {
        game.turn = 15;
        game.players.at(0).jailed = true;
        game.players.at(0).position = 6;
    } else {
        game.turn = 28;
        if (game.players.size() > 1) {
            game.players.resize(2);
        }
    }
}

std::vector<TileInfo> GUIGameController::createBoard() const {
    return {
        {0, "GO", "GO", "Mulai perjalananmu di sini.", "", TileKind::Go, toolkit.theme().gold, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {1, "MTR", "Menteng", "Street awal dengan harga terjangkau.", "Amber", TileKind::Street, toolkit.colorForGroup("Amber"), 100, 50, 8, 50, 100, -1, 0, 0, false},
        {2, "CC1", "Dana Umum", "Ambil kartu komunitas.", "", TileKind::CommunityChest, toolkit.theme().sage, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {3, "SNT", "Senen", "Street awal lain untuk membuka set pertama.", "Amber", TileKind::Street, toolkit.colorForGroup("Amber"), 120, 60, 10, 50, 100, -1, 0, 0, false},
        {4, "PPH", "PPh", "Bayar pajak penghasilan.", "", TileKind::IncomeTax, toolkit.theme().coral, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {5, "KRL", "KRL", "Railroad pertama di board.", "", TileKind::Railroad, toolkit.theme().navy, 200, 100, 25, 0, 0, -1, 0, 0, false},
        {6, "JIL", "Jail", "Tempat singgah atau hukuman.", "", TileKind::Jail, toolkit.theme().danger, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {7, "CHA", "Chance", "Ambil kartu chance.", "", TileKind::Chance, toolkit.theme().teal, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {8, "BDG", "Bandung", "Street menengah dengan potensi sewa yang naik.", "Cyan", TileKind::Street, toolkit.colorForGroup("Cyan"), 140, 70, 12, 50, 100, -1, 0, 0, false},
        {9, "PLM", "Palmerah", "Street menengah pelengkap set cyan.", "Cyan", TileKind::Street, toolkit.colorForGroup("Cyan"), 160, 80, 14, 50, 100, -1, 0, 0, false},
        {10, "PLN", "PLN", "Utility pertama.", "", TileKind::Utility, toolkit.theme().teal, 150, 75, 40, 0, 0, -1, 0, 0, false},
        {11, "FES", "Festival", "Pilih properti untuk boost festival.", "", TileKind::Festival, toolkit.theme().coral, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {12, "PRA", "Pramuka", "Street transisi menuju mid board.", "Jade", TileKind::Street, toolkit.colorForGroup("Jade"), 180, 90, 16, 100, 200, -1, 0, 0, false},
        {13, "CC2", "Dana Umum", "Ambil kartu komunitas lain.", "", TileKind::CommunityChest, toolkit.theme().sage, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {14, "TMN", "Taman", "Street jade kedua.", "Jade", TileKind::Street, toolkit.colorForGroup("Jade"), 200, 100, 18, 100, 200, -1, 0, 0, false},
        {15, "PBM", "PBM", "Bayar pajak barang mewah.", "", TileKind::LuxuryTax, toolkit.theme().coral, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {16, "KAI", "KAI", "Railroad kedua.", "", TileKind::Railroad, toolkit.theme().navy, 200, 100, 25, 0, 0, -1, 0, 0, false},
        {17, "GJL", "Go To Jail", "Langsung ke jail jika mendarat di sini.", "", TileKind::GoToJail, toolkit.theme().danger, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {18, "CH2", "Chance", "Chance kedua.", "", TileKind::Chance, toolkit.theme().teal, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {19, "KNN", "Kuningan", "Street premium pertama.", "Scarlet", TileKind::Street, toolkit.colorForGroup("Scarlet"), 320, 160, 34, 120, 240, -1, 0, 0, false},
        {20, "DU2", "Dana Sosial", "Komunitas lain dengan efek kejutan.", "", TileKind::CommunityChest, toolkit.theme().sage, 0, 0, 0, 0, 0, -1, 0, 0, false},
        {21, "THM", "Thamrin", "Street premium kedua.", "Scarlet", TileKind::Street, toolkit.colorForGroup("Scarlet"), 340, 170, 38, 120, 240, -1, 0, 0, false},
        {22, "PAM", "PAM", "Utility kedua.", "", TileKind::Utility, toolkit.theme().teal, 150, 75, 40, 0, 0, -1, 0, 0, false},
        {23, "PRK", "Free Parking", "Tempat istirahat sementara.", "", TileKind::FreeParking, toolkit.theme().paper, 0, 0, 0, 0, 0, -1, 0, 0, false},
    };
}

std::vector<SaveSlot> GUIGameController::createInitialSaveSlots() const {
    return {
        {"Save 1", "Permainan tengah dengan aset tersebar", 7, 4, toolkit.theme().coral},
        {"Save 2", "Pemain berada di jail dan aset tergadai", 15, 3, toolkit.theme().teal},
        {"Save 3", "Duel akhir menjelang batas turn", 28, 2, toolkit.theme().gold},
    };
}
