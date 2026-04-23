#include "controller/GUIController/GUIGameController.hpp"

#include "view/raylib/UiToolkit.hpp"

#include <algorithm>
#include <ctime>

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
    const GameState& game = appState_.getGame();
    if (playerIndex < 0 || playerIndex >= static_cast<int>(game.getPlayers().size())) {
        return 0;
    }

    const PlayerInfo& player = game.getPlayers().at(playerIndex);
    int wealth = player.getMoney();
    for (int tileIndex : player.getProperties()) {
        wealth += computeTileAssetValue(game.getBoard().at(tileIndex));
    }
    wealth += static_cast<int>(player.getHandCards().size()) * 75;
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
    appState_.setGame(createBaseGame());
    appState_.setScreen(Screen::Gameplay);
}

void GUIGameController::loadSessionFromSlot(int slotIndex) {
    appState_.setSelectedSave(slotIndex);
    appState_.setGame(createBaseGame());
    applyScenario(appState_.getGame(), slotIndex);
    appState_.setScreen(Screen::Gameplay);
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
    GameState& game = appState_.getGame();
    game.getLogs().push_back({game.getTurn(), actor, action, detail});
    if (static_cast<int>(game.getLogs().size()) > kMaxLogEntries) {
        game.getLogs().erase(game.getLogs().begin());
    }
}

void GUIGameController::maybeOpenLiquidation() {
    const GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    if (game.getPlayers().at(game.getCurrentPlayer()).getMoney() < 0) {
        openLiquidation();
    }
}

GameState GUIGameController::createBaseGame() const {
    GameState game;
    game.setSessionLabel("NIMONSPOLI");
    game.setBoard(createBoard());
    game.setTurn(1);
    game.setTurnLimit(std::max(12, appState_.getTurnLimit()));
    game.setCurrentPlayer(0);
    game.setSelectedTile(0);
    game.setStartCash(appState_.getStartingCash());

    for (int index = 0; index < appState_.getPlayerCount(); index++) {
        PlayerInfo player;
        player.setName(appState_.getPlayerNames().at(index));
        player.setAccent(playerAccent(index));
        player.setMoney(appState_.getStartingCash());
        player.setPosition(0);
        game.getPlayers().push_back(player);
    }

    return game;
}

void GUIGameController::applyScenario(GameState& game, int scenarioIndex) const {
    if (game.getPlayers().empty()) {
        return;
    }

    if (scenarioIndex == 0) {
        game.setTurn(7);
    } else if (scenarioIndex == 1) {
        game.setTurn(15);
        game.getPlayers().at(0).setJailed(true);
        game.getPlayers().at(0).setPosition(6);
    } else {
        game.setTurn(28);
        if (game.getPlayers().size() > 1) {
            game.getPlayers().resize(2);
        }
    }
}

std::vector<TileInfo> GUIGameController::createBoard() const {
    return {
        {0, "GO", "GO", "Mulai perjalananmu di sini.", "", TileKind::Go, toolkit.theme().getGold(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {1, "MTR", "Menteng", "Street awal dengan harga terjangkau.", "Amber", TileKind::Street, toolkit.colorForGroup("Amber"), 100, 50, 8, 50, 100, -1, 0, 0, false},
        {2, "CC1", "Dana Umum", "Ambil kartu komunitas.", "", TileKind::CommunityChest, toolkit.theme().getSage(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {3, "SNT", "Senen", "Street awal lain untuk membuka set pertama.", "Amber", TileKind::Street, toolkit.colorForGroup("Amber"), 120, 60, 10, 50, 100, -1, 0, 0, false},
        {4, "PPH", "PPh", "Bayar pajak penghasilan.", "", TileKind::IncomeTax, toolkit.theme().getCoral(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {5, "KRL", "KRL", "Railroad pertama di board.", "", TileKind::Railroad, toolkit.theme().getNavy(), 200, 100, 25, 0, 0, -1, 0, 0, false},
        {6, "JIL", "Jail", "Tempat singgah atau hukuman.", "", TileKind::Jail, toolkit.theme().getDanger(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {7, "CHA", "Chance", "Ambil kartu chance.", "", TileKind::Chance, toolkit.theme().getTeal(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {8, "BDG", "Bandung", "Street menengah dengan potensi sewa yang naik.", "Cyan", TileKind::Street, toolkit.colorForGroup("Cyan"), 140, 70, 12, 50, 100, -1, 0, 0, false},
        {9, "PLM", "Palmerah", "Street menengah pelengkap set cyan.", "Cyan", TileKind::Street, toolkit.colorForGroup("Cyan"), 160, 80, 14, 50, 100, -1, 0, 0, false},
        {10, "PLN", "PLN", "Utility pertama.", "", TileKind::Utility, toolkit.theme().getTeal(), 150, 75, 40, 0, 0, -1, 0, 0, false},
        {11, "FES", "Festival", "Pilih properti untuk boost festival.", "", TileKind::Festival, toolkit.theme().getCoral(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {12, "PRA", "Pramuka", "Street transisi menuju mid board.", "Jade", TileKind::Street, toolkit.colorForGroup("Jade"), 180, 90, 16, 100, 200, -1, 0, 0, false},
        {13, "CC2", "Dana Umum", "Ambil kartu komunitas lain.", "", TileKind::CommunityChest, toolkit.theme().getSage(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {14, "TMN", "Taman", "Street jade kedua.", "Jade", TileKind::Street, toolkit.colorForGroup("Jade"), 200, 100, 18, 100, 200, -1, 0, 0, false},
        {15, "PBM", "PBM", "Bayar pajak barang mewah.", "", TileKind::LuxuryTax, toolkit.theme().getCoral(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {16, "KAI", "KAI", "Railroad kedua.", "", TileKind::Railroad, toolkit.theme().getNavy(), 200, 100, 25, 0, 0, -1, 0, 0, false},
        {17, "GJL", "Go To Jail", "Langsung ke jail jika mendarat di sini.", "", TileKind::GoToJail, toolkit.theme().getDanger(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {18, "CH2", "Chance", "Chance kedua.", "", TileKind::Chance, toolkit.theme().getTeal(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {19, "KNN", "Kuningan", "Street premium pertama.", "Scarlet", TileKind::Street, toolkit.colorForGroup("Scarlet"), 320, 160, 34, 120, 240, -1, 0, 0, false},
        {20, "DU2", "Dana Sosial", "Komunitas lain dengan efek kejutan.", "", TileKind::CommunityChest, toolkit.theme().getSage(), 0, 0, 0, 0, 0, -1, 0, 0, false},
        {21, "THM", "Thamrin", "Street premium kedua.", "Scarlet", TileKind::Street, toolkit.colorForGroup("Scarlet"), 340, 170, 38, 120, 240, -1, 0, 0, false},
        {22, "PAM", "PAM", "Utility kedua.", "", TileKind::Utility, toolkit.theme().getTeal(), 150, 75, 40, 0, 0, -1, 0, 0, false},
        {23, "PRK", "Free Parking", "Tempat istirahat sementara.", "", TileKind::FreeParking, toolkit.theme().getPaper(), 0, 0, 0, 0, 0, -1, 0, 0, false},
    };
}

std::vector<SaveSlot> GUIGameController::createInitialSaveSlots() const {
    return {
        {"Save 1", "Permainan tengah dengan aset tersebar", 7, 4, toolkit.theme().getCoral()},
        {"Save 2", "Pemain berada di jail dan aset tergadai", 15, 3, toolkit.theme().getTeal()},
        {"Save 3", "Duel akhir menjelang batas turn", 28, 2, toolkit.theme().getGold()},
    };
}
