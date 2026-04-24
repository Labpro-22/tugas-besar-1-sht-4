#include "controller/GUIController/GUIGameController.hpp"

#include "view/raylib/UiToolkit.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
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

int countOwnedByKind(const GameState& game, int ownerIndex, TileKind kind) {
    int total = 0;
    for (const TileInfo& tile : game.getBoard()) {
        if (tile.getOwnerIndex() == ownerIndex && tile.getKind() == kind) {
            total++;
        }
    }
    return total;
}

int computeStreetRent(const GameState& game, const TileInfo& tile) {
    int rent = tile.getBaseRent();
    if (tile.getBuildings() == 0 &&
        tile.getOwnerIndex() >= 0 &&
        playerOwnsFullStreetGroup(game, tile.getOwnerIndex(), tile)) {
        rent *= 2;
    }
    if (tile.getBuildings() == 1) rent = tile.getBaseRent() * 2;
    if (tile.getBuildings() == 2) rent = tile.getBaseRent() * 4;
    if (tile.getBuildings() == 3) rent = tile.getBaseRent() * 7;
    if (tile.getBuildings() == 4) rent = tile.getBaseRent() * 10;
    if (tile.getBuildings() >= 5) rent = tile.getBaseRent() * 14;
    return tile.getFestivalTurns() > 0 ? rent + rent / 2 : rent;
}

void removePropertyReference(PlayerInfo& player, int tileIndex) {
    player.getProperties().erase(
        std::remove(player.getProperties().begin(), player.getProperties().end(), tileIndex),
        player.getProperties().end()
    );
}

void assignTileToPlayer(GameState& game, int tileIndex, int playerIndex) {
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
    if (std::find(game.getPlayers().at(playerIndex).getProperties().begin(),
                  game.getPlayers().at(playerIndex).getProperties().end(),
                  tileIndex) == game.getPlayers().at(playerIndex).getProperties().end()) {
        game.getPlayers().at(playerIndex).getProperties().push_back(tileIndex);
    }
}

void releaseTile(GameState& game, int tileIndex) {
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

int findFirstTile(const GameState& game, TileKind kind) {
    for (const TileInfo& tile : game.getBoard()) {
        if (tile.getKind() == kind) {
            return tile.getIndex();
        }
    }
    return -1;
}

int findJailTile(const GameState& game) {
    for (const TileInfo& tile : game.getBoard()) {
        if (tile.getKind() == TileKind::Jail) {
            return tile.getIndex();
        }
    }
    return 6;
}

std::vector<CardInfo> chanceDeck() {
    return {
        {"Teleport", "Pindah ke tile terpilih.", CardKind::Hand, CardEffect::TeleportAnywhere, 0, SKYBLUE},
        {"Go To Jail", "Masuk jail.", CardKind::Instant, CardEffect::GoToJail, 0, RED},
        {"Move Back 3", "Mundur tiga langkah.", CardKind::Instant, CardEffect::MoveBackThree, 3, ORANGE},
    };
}

std::vector<CardInfo> communityDeck() {
    return {
        {"Birthday Card", "Tambah uang.", CardKind::Instant, CardEffect::GainMoney, 150, GREEN},
        {"Doctor Fee", "Kurangi uang.", CardKind::Instant, CardEffect::LoseMoney, 75, MAROON},
        {"Shield", "Lindungi dari jail.", CardKind::Hand, CardEffect::ActivateShield, 0, DARKBLUE},
    };
}

CardInfo pickRandomCard(AppState& state, int deckKey) {
    const std::vector<CardInfo> cards = deckKey == kCommunityDeckKey ? communityDeck() : chanceDeck();
    if (cards.empty()) {
        return {};
    }

    std::uniform_int_distribution<int> distribution(0, static_cast<int>(cards.size()) - 1);
    return cards.at(distribution(state.getRng()));
}

bool currentPlayerNeedsForceDrop(const GameState& game) {
    if (game.getPlayers().empty()) {
        return false;
    }

    return game.getPlayers().at(game.getCurrentPlayer()).getHandCards().size() > 3U;
}

bool isJailCard(const CardInfo& card) {
    std::string title = card.getTitle();
    std::transform(title.begin(), title.end(), title.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return card.getEffect() == CardEffect::ActivateShield ||
           title.find("jail") != std::string::npos ||
           title.find("bebas") != std::string::npos;
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
    appState_.setOverlay(OverlayState{});
    appState_.getActiveField().clear();
}

bool GUIGameController::isOverlayOpen() const {
    return appState_.getOverlay().getType() != OverlayType::None;
}

int GUIGameController::getMortgageValue(const TileInfo& tile) const {
    return tile.getMortgageValue() > 0 ? tile.getMortgageValue() : tile.getPrice() / 2;
}

int GUIGameController::getRedeemCost(const TileInfo& tile) const {
    const int mortgageValue = getMortgageValue(tile);
    return mortgageValue + mortgageValue / 10;
}

int GUIGameController::findJailIndex() const {
    return findJailTile(appState_.getGame());
}

int GUIGameController::computeRent(const TileInfo& tile) const {
    if (tile.isMortgaged() || tile.getOwnerIndex() < 0) {
        return 0;
    }

    if (tile.getKind() == TileKind::Street) {
        return computeStreetRent(appState_.getGame(), tile);
    }
    if (tile.getKind() == TileKind::Railroad) {
        return 25 * std::max(1, countOwnedByKind(appState_.getGame(), tile.getOwnerIndex(), TileKind::Railroad));
    }
    if (tile.getKind() == TileKind::Utility) {
        return countOwnedByKind(appState_.getGame(), tile.getOwnerIndex(), TileKind::Utility) >= 2 ? 90 : 40;
    }

    return 0;
}

int GUIGameController::computeTileAssetValue(const TileInfo& tile) const {
    int total = tile.getPrice();
    if (tile.getKind() == TileKind::Street) {
        total += std::min(tile.getBuildings(), 4) * tile.getHouseCost();
        if (tile.getBuildings() >= 5) {
            total += tile.getHotelCost();
        }
    }
    return total;
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

void GUIGameController::setSelectedTile(int tileIndex) {
    appState_.getGame().setSelectedTile(tileIndex);
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
    appState_.getOverlay().setTileIndex(findFirstTile(appState_.getGame(), TileKind::IncomeTax));
    openOverlay(OverlayType::IncomeTax);
}

void GUIGameController::openLuxuryTax() {
    appState_.getOverlay().setTileIndex(findFirstTile(appState_.getGame(), TileKind::LuxuryTax));
    openOverlay(OverlayType::LuxuryTax);
}

void GUIGameController::openFestival() {
    appState_.getOverlay().setTileIndex(findFirstTile(appState_.getGame(), TileKind::Festival));
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
    appState_.getOverlay().setDeckKey(deckKey);
    appState_.getOverlay().setCard(pickRandomCard(appState_, deckKey));
    openOverlay(OverlayType::CardDraw);
}

void GUIGameController::openGameOver() {
    openOverlay(OverlayType::GameOver);
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
    GameState& game = appState_.getGame();
    if (appState_.getScreen() != Screen::Gameplay || game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.isBankrupt()) {
        addToast("Pemain ini sudah bangkrut.", RED);
        return;
    }

    game.setTurnStarted(true);
    game.setRolledThisTurn(false);
    addLog(player.getName(), "Start Turn", "Memulai giliran.");

    if (player.isJailed()) {
        openJail();
    } else if (currentPlayerNeedsForceDrop(game)) {
        openForceDrop();
    }
}

void GUIGameController::rollDice() {
    GameState& game = appState_.getGame();
    if (appState_.getScreen() != Screen::Gameplay || game.getPlayers().empty()) {
        return;
    }

    if (!game.isTurnStarted()) {
        addToast("Mulai turn dulu sebelum melempar dadu.", RED);
        return;
    }
    if (game.isRolledThisTurn()) {
        addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.isJailed()) {
        openJail();
        addToast("Pemain masih di jail.", RED);
        return;
    }

    std::uniform_int_distribution<int> distribution(1, 6);
    game.setLastDieOne(distribution(appState_.getRng()));
    game.setLastDieTwo(distribution(appState_.getRng()));
    game.setRolledThisTurn(true);

    const int steps = game.getLastDieOne() + game.getLastDieTwo();
    const int previousPosition = player.getPosition();
    const int boardSize = static_cast<int>(game.getBoard().size());
    const int nextPosition = boardSize > 0 ? (previousPosition + steps) % boardSize : previousPosition;
    if (boardSize > 0 && previousPosition + steps >= boardSize) {
        player.setMoney(player.getMoney() + 200);
        addToast("Melewati GO: +200.", GOLD);
    }

    player.setPosition(nextPosition);
    addLog(
        player.getName(),
        "Roll Dice",
        "Melempar " + std::to_string(game.getLastDieOne()) + " dan " +
            std::to_string(game.getLastDieTwo()) + ", mendarat di " +
            game.getBoard().at(nextPosition).getName() + "."
    );
    addToast(
        "Dadu " + std::to_string(game.getLastDieOne()) + " + " + std::to_string(game.getLastDieTwo()),
        player.getAccent()
    );
    triggerTileEvent(nextPosition, false);
}

void GUIGameController::applyManualDice() {
    GameState& game = appState_.getGame();
    if (appState_.getScreen() != Screen::Gameplay || game.getPlayers().empty()) {
        return;
    }

    if (!game.isTurnStarted()) {
        addToast("Mulai turn dulu sebelum mengatur dadu.", RED);
        return;
    }
    if (game.isRolledThisTurn()) {
        addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.isJailed()) {
        openJail();
        addToast("Pemain masih di jail.", RED);
        return;
    }

    const OverlayState& overlay = appState_.getOverlay();
    game.setLastDieOne(overlay.getManualDieOne());
    game.setLastDieTwo(overlay.getManualDieTwo());
    game.setRolledThisTurn(true);
    closeOverlay();

    const int steps = game.getLastDieOne() + game.getLastDieTwo();
    const int previousPosition = player.getPosition();
    const int boardSize = static_cast<int>(game.getBoard().size());
    const int nextPosition = boardSize > 0 ? (previousPosition + steps) % boardSize : previousPosition;
    if (boardSize > 0 && previousPosition + steps >= boardSize) {
        player.setMoney(player.getMoney() + 200);
        addToast("Melewati GO: +200.", GOLD);
    }

    player.setPosition(nextPosition);
    addLog(
        player.getName(),
        "Set Dice",
        "Menggunakan dadu manual " + std::to_string(game.getLastDieOne()) + " dan " +
            std::to_string(game.getLastDieTwo()) + ", mendarat di " +
            game.getBoard().at(nextPosition).getName() + "."
    );
    addToast("Dadu manual diterapkan.", player.getAccent());
    triggerTileEvent(nextPosition, false);
}

void GUIGameController::endTurn() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    if (game.getPlayers().at(game.getCurrentPlayer()).getMoney() < 0) {
        maybeOpenLiquidation();
        return;
    }

    int activePlayers = 0;
    for (TileInfo& tile : game.getBoard()) {
        if (tile.getFestivalTurns() > 0) {
            tile.setFestivalTurns(tile.getFestivalTurns() - 1);
        }
    }
    for (const PlayerInfo& player : game.getPlayers()) {
        if (!player.isBankrupt()) {
            activePlayers++;
        }
    }
    if (activePlayers <= 1) {
        openGameOver();
        return;
    }

    int nextPlayer = game.getCurrentPlayer();
    for (int checked = 0; checked < static_cast<int>(game.getPlayers().size()); checked++) {
        nextPlayer = (nextPlayer + 1) % static_cast<int>(game.getPlayers().size());
        if (!game.getPlayers().at(nextPlayer).isBankrupt()) {
            break;
        }
    }

    game.getPlayers().at(game.getCurrentPlayer()).setDiscountPercent(0);
    game.setCurrentPlayer(nextPlayer);
    game.setTurn(game.getTurn() + 1);
    game.setTurnStarted(false);
    game.setRolledThisTurn(false);
    if (game.getTurnLimit() > 0 && game.getTurn() > game.getTurnLimit()) {
        openGameOver();
    }
}

void GUIGameController::triggerTileEvent(int tileIndex, bool fromMovement) {
    GameState& game = appState_.getGame();
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
                addToast("Mendarat di properti sendiri.", tile.getAccent());
                return;
            }

            const int rent = computeRent(tile);
            if (rent <= 0) {
                addToast("Properti ini tidak menarik sewa.", tile.getAccent());
                return;
            }

            PlayerInfo& owner = game.getPlayers().at(tile.getOwnerIndex());
            owner.setMoney(owner.getMoney() + rent);
            player.setMoney(player.getMoney() - rent);
            addLog(player.getName(), "Bayar Rent", "Membayar " + std::to_string(rent) + " ke " + owner.getName() + ".");
            addToast("Bayar rent " + std::to_string(rent) + ".", tile.getAccent());
            maybeOpenLiquidation();
            return;
        }
        case TileKind::Chance:
            openRandomCardDraw(kChanceDeckKey);
            return;
        case TileKind::CommunityChest:
            openRandomCardDraw(kCommunityDeckKey);
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
            if (player.isShieldActive()) {
                player.setShieldActive(false);
                addLog(player.getName(), "Shield", "Shield aktif menahan efek jail.");
                addToast("Shield menahan efek jail.", player.getAccent());
                return;
            }
            player.setJailed(true);
            player.setPosition(findJailIndex());
            player.setFailedJailRolls(0);
            addLog(player.getName(), "Go To Jail", fromMovement ? "Kartu atau perpindahan" : "Petak Go To Jail");
            addToast(player.getName() + " masuk jail.", RED);
            openJail();
            return;
        case TileKind::Go:
            addToast("Mendarat di GO.", tile.getAccent());
            return;
        case TileKind::Jail:
            addToast(player.isJailed() ? "Masih berada di jail." : "Hanya berkunjung ke jail.", tile.getAccent());
            return;
        case TileKind::FreeParking:
            addToast("Istirahat sejenak di Free Parking.", tile.getAccent());
            return;
    }
}

bool GUIGameController::canSaveNow() const {
    return appState_.getGame().isTurnStarted() && !appState_.getGame().isRolledThisTurn();
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
    const GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return false;
    }

    return game.getPlayers().at(game.getCurrentPlayer()).getMoney() >= currentPurchasePrice();
}

int GUIGameController::currentPurchasePrice() const {
    const GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return 0;
    }

    const int tileIndex = appState_.getOverlay().getTileIndex();
    if (tileIndex < 0 || tileIndex >= static_cast<int>(game.getBoard().size())) {
        return 0;
    }

    const TileInfo& tile = game.getBoard().at(tileIndex);
    const PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int discount = player.getDiscountPercent();
    return discount > 0 ? tile.getPrice() * (100 - discount) / 100 : tile.getPrice();
}

void GUIGameController::buySelectedProperty() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    const int tileIndex = appState_.getOverlay().getTileIndex();
    if (tileIndex < 0 || tileIndex >= static_cast<int>(game.getBoard().size())) {
        return;
    }

    TileInfo& tile = game.getBoard().at(tileIndex);
    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (!isOwnable(tile.getKind()) || tile.getOwnerIndex() >= 0) {
        addToast("Properti ini tidak bisa dibeli.", RED);
        return;
    }

    const int price = currentPurchasePrice();
    if (player.getMoney() < price) {
        addToast("Saldo tidak cukup untuk membeli properti.", RED);
        return;
    }

    player.setMoney(player.getMoney() - price);
    assignTileToPlayer(game, tileIndex, game.getCurrentPlayer());
    player.setDiscountPercent(0);
    addLog(player.getName(), "Beli Properti", "Membeli " + tile.getName() + " seharga " + std::to_string(price) + ".");
    addToast("Berhasil membeli " + tile.getName() + ".", tile.getAccent());
    closeOverlay();
}

void GUIGameController::skipSelectedPurchase() {
    closeOverlay();
}

void GUIGameController::auctionRaiseBid(int amount) {
    GameState& game = appState_.getGame();
    AuctionState& auction = appState_.getOverlay().getAuction();
    const int bidderIndex = auction.getSelectedBidder();
    if (bidderIndex < 0 || bidderIndex >= static_cast<int>(game.getPlayers().size())) {
        return;
    }
    if (bidderIndex < static_cast<int>(auction.getPassed().size()) && auction.getPassed().at(bidderIndex)) {
        addToast("Bidder ini sudah pass.", RED);
        return;
    }

    PlayerInfo& bidder = game.getPlayers().at(bidderIndex);
    if (bidder.isBankrupt()) {
        addToast("Pemain bangkrut tidak bisa ikut bid.", RED);
        return;
    }

    const int nextBid = auction.getHighestBid() + amount;
    if (bidder.getMoney() < nextBid) {
        addToast("Saldo bidder tidak cukup.", RED);
        return;
    }

    auction.setHighestBid(nextBid);
    auction.setHighestBidder(bidderIndex);
    addLog(bidder.getName(), "Auction Bid", "Menaikkan bid menjadi " + std::to_string(nextBid) + ".");
    addToast("Bid " + std::to_string(nextBid) + " oleh " + bidder.getName() + ".", bidder.getAccent());

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

void GUIGameController::auctionPass() {
    GameState& game = appState_.getGame();
    AuctionState& auction = appState_.getOverlay().getAuction();
    if (auction.getSelectedBidder() < static_cast<int>(auction.getPassed().size())) {
        auction.getPassed().at(auction.getSelectedBidder()) = true;
        addLog(
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

void GUIGameController::finalizeAuction() {
    GameState& game = appState_.getGame();
    AuctionState& auction = appState_.getOverlay().getAuction();
    const int tileIndex = auction.getTileIndex();
    if (tileIndex >= 0 && tileIndex < static_cast<int>(game.getBoard().size()) &&
        auction.getHighestBidder() >= 0 &&
        auction.getHighestBidder() < static_cast<int>(game.getPlayers().size())) {
        TileInfo& tile = game.getBoard().at(tileIndex);
        PlayerInfo& winner = game.getPlayers().at(auction.getHighestBidder());
        if (!winner.isBankrupt() && winner.getMoney() >= auction.getHighestBid()) {
            winner.setMoney(winner.getMoney() - auction.getHighestBid());
            assignTileToPlayer(game, tileIndex, auction.getHighestBidder());
            addLog(
                winner.getName(),
                "Auction Win",
                "Memenangkan " + tile.getName() + " dengan bid " + std::to_string(auction.getHighestBid()) + "."
            );
            addToast("Lelang dimenangkan " + winner.getName() + ".", winner.getAccent());
        }
    }
    closeOverlay();
}

int GUIGameController::flatIncomeTax() const {
    return 200;
}

int GUIGameController::percentageIncomeTax() const {
    return std::max(120, computeNetWorth(appState_.getGame().getCurrentPlayer()) / 10);
}

void GUIGameController::payIncomeTax(bool useFlatTax) {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int tax = useFlatTax ? flatIncomeTax() : percentageIncomeTax();
    player.setMoney(player.getMoney() - tax);
    addLog(player.getName(), "Bayar PPh", "Membayar pajak sebesar " + std::to_string(tax) + ".");
    addToast("PPh dibayar.", RED);
    closeOverlay();
    maybeOpenLiquidation();
}

int GUIGameController::luxuryTaxAmount() const {
    return 150;
}

void GUIGameController::payLuxuryTax() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int tax = luxuryTaxAmount();
    player.setMoney(player.getMoney() - tax);
    addLog(player.getName(), "Bayar PBM", "Membayar pajak mewah sebesar " + std::to_string(tax) + ".");
    addToast("PBM dibayar.", RED);
    closeOverlay();
    maybeOpenLiquidation();
}

void GUIGameController::activateFestivalOnSelectedTile() {
    GameState& game = appState_.getGame();
    const std::vector<int> options = currentPlayerStreetOptions();
    if (options.empty() || game.getPlayers().empty()) {
        addToast("Belum ada street untuk festival.", RED);
        return;
    }

    const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));
    tile.setFestivalTurns(3);
    addLog(game.getPlayers().at(game.getCurrentPlayer()).getName(), "Festival", "Mengaktifkan festival di " + tile.getName() + ".");
    addToast("Festival aktif di " + tile.getName() + ".", tile.getAccent());
    closeOverlay();
}

void GUIGameController::payJailFine() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int fine = 100;
    if (player.getMoney() < fine) {
        addToast("Saldo tidak cukup untuk membayar denda jail.", RED);
        openLiquidation();
        return;
    }

    player.setMoney(player.getMoney() - fine);
    player.setJailed(false);
    player.setFailedJailRolls(0);
    addLog(player.getName(), "Keluar Jail", "Membayar denda sebesar 100.");
    addToast("Denda jail dibayar.", player.getAccent());
    closeOverlay();
}

void GUIGameController::useJailCard() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (!player.isJailed()) {
        addToast("Pemain ini tidak sedang di jail.", RED);
        return;
    }

    const auto found = std::find_if(player.getHandCards().begin(), player.getHandCards().end(), isJailCard);
    if (found == player.getHandCards().end()) {
        addToast("Tidak ada kartu bebas jail yang bisa dipakai.", RED);
        return;
    }

    const CardInfo usedCard = *found;
    player.getHandCards().erase(found);
    player.setJailed(false);
    player.setFailedJailRolls(0);
    addLog(player.getName(), "Use Jail Card", "Menggunakan " + usedCard.getTitle() + " untuk keluar dari jail.");
    addToast("Keluar dari jail dengan kartu.", usedCard.getAccent());
    closeOverlay();
}

void GUIGameController::useSelectedHandCard() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.getHandCards().empty()) {
        addToast("Belum ada kartu di tangan.", RED);
        return;
    }

    const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(player.getHandCards().size()) - 1));
    const CardInfo card = player.getHandCards().at(selected);
    player.getHandCards().erase(player.getHandCards().begin() + selected);
    closeOverlay();

    addLog(player.getName(), "Use Card", card.getTitle() + ": " + card.getDescription());
    switch (card.getEffect()) {
        case CardEffect::GainMoney:
            player.setMoney(player.getMoney() + card.getMagnitude());
            addToast("Uang bertambah " + std::to_string(card.getMagnitude()) + ".", card.getAccent());
            break;
        case CardEffect::LoseMoney:
            player.setMoney(player.getMoney() - card.getMagnitude());
            addToast("Uang berkurang " + std::to_string(card.getMagnitude()) + ".", card.getAccent());
            maybeOpenLiquidation();
            break;
        case CardEffect::MoveToGo:
            player.setPosition(0);
            player.setMoney(player.getMoney() + 200);
            addToast("Pindah ke GO.", card.getAccent());
            triggerTileEvent(0, true);
            break;
        case CardEffect::MoveBackThree: {
            const int boardSize = static_cast<int>(game.getBoard().size());
            if (boardSize <= 0) {
                break;
            }
            const int nextPosition = (player.getPosition() - std::max(1, card.getMagnitude()) + boardSize) % boardSize;
            player.setPosition(nextPosition);
            addToast("Mundur ke " + game.getBoard().at(nextPosition).getName() + ".", card.getAccent());
            triggerTileEvent(nextPosition, true);
            break;
        }
        case CardEffect::GoToJail:
            if (player.isShieldActive()) {
                player.setShieldActive(false);
                addToast("Shield aktif menahan efek jail.", card.getAccent());
                break;
            }
            player.setJailed(true);
            player.setFailedJailRolls(0);
            player.setPosition(findJailIndex());
            addToast("Masuk jail.", RED);
            openJail();
            break;
        case CardEffect::TeleportAnywhere: {
            const int destination = std::max(0, std::min(game.getSelectedTile(), static_cast<int>(game.getBoard().size()) - 1));
            player.setPosition(destination);
            addToast("Teleport ke " + game.getBoard().at(destination).getName() + ".", card.getAccent());
            triggerTileEvent(destination, true);
            break;
        }
        case CardEffect::ActivateShield:
            if (player.isJailed()) {
                player.setJailed(false);
                player.setFailedJailRolls(0);
                addToast("Kartu membebaskan pemain dari jail.", card.getAccent());
            } else {
                player.setShieldActive(true);
                addToast("Shield aktif untuk pemain ini.", card.getAccent());
            }
            break;
        case CardEffect::ActivateDiscount:
            player.setDiscountPercent(card.getMagnitude() > 0 ? card.getMagnitude() : 20);
            addToast("Diskon pembelian aktif.", card.getAccent());
            break;
    }
}

void GUIGameController::storeDrawnCard() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const CardInfo card = appState_.getOverlay().getCard();
    player.getHandCards().push_back(card);
    addLog(player.getName(), "Draw Card", "Menyimpan kartu " + card.getTitle() + " ke tangan.");
    addToast("Kartu disimpan ke tangan.", card.getAccent());
    closeOverlay();
    if (currentPlayerNeedsForceDrop(game)) {
        openForceDrop();
    }
}

void GUIGameController::applyDrawnCard() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const CardInfo card = appState_.getOverlay().getCard();
    closeOverlay();

    addLog(player.getName(), "Apply Card", card.getTitle() + ": " + card.getDescription());
    switch (card.getEffect()) {
        case CardEffect::GainMoney:
            player.setMoney(player.getMoney() + card.getMagnitude());
            addToast("Uang bertambah " + std::to_string(card.getMagnitude()) + ".", card.getAccent());
            break;
        case CardEffect::LoseMoney:
            player.setMoney(player.getMoney() - card.getMagnitude());
            addToast("Uang berkurang " + std::to_string(card.getMagnitude()) + ".", card.getAccent());
            maybeOpenLiquidation();
            break;
        case CardEffect::MoveToGo:
            player.setPosition(0);
            player.setMoney(player.getMoney() + 200);
            addToast("Pindah ke GO.", card.getAccent());
            triggerTileEvent(0, true);
            break;
        case CardEffect::MoveBackThree: {
            const int boardSize = static_cast<int>(game.getBoard().size());
            if (boardSize <= 0) {
                break;
            }
            const int nextPosition = (player.getPosition() - std::max(1, card.getMagnitude()) + boardSize) % boardSize;
            player.setPosition(nextPosition);
            addToast("Mundur ke " + game.getBoard().at(nextPosition).getName() + ".", card.getAccent());
            triggerTileEvent(nextPosition, true);
            break;
        }
        case CardEffect::GoToJail:
            if (player.isShieldActive()) {
                player.setShieldActive(false);
                addToast("Shield aktif menahan efek jail.", card.getAccent());
                break;
            }
            player.setJailed(true);
            player.setFailedJailRolls(0);
            player.setPosition(findJailIndex());
            addToast("Masuk jail.", RED);
            openJail();
            break;
        case CardEffect::TeleportAnywhere: {
            const int destination = std::max(0, std::min(game.getSelectedTile(), static_cast<int>(game.getBoard().size()) - 1));
            player.setPosition(destination);
            addToast("Teleport ke " + game.getBoard().at(destination).getName() + ".", card.getAccent());
            triggerTileEvent(destination, true);
            break;
        }
        case CardEffect::ActivateShield:
            if (player.isJailed()) {
                player.setJailed(false);
                player.setFailedJailRolls(0);
                addToast("Kartu membebaskan pemain dari jail.", card.getAccent());
            } else {
                player.setShieldActive(true);
                addToast("Shield aktif untuk pemain ini.", card.getAccent());
            }
            break;
        case CardEffect::ActivateDiscount:
            player.setDiscountPercent(card.getMagnitude() > 0 ? card.getMagnitude() : 20);
            addToast("Diskon pembelian aktif.", card.getAccent());
            break;
    }
}

void GUIGameController::buildOnSelectedTile() {
    GameState& game = appState_.getGame();
    const std::vector<int> options = currentPlayerBuildOptions();
    if (options.empty() || game.getPlayers().empty()) {
        addToast("Tidak ada properti yang bisa dibangun.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));
    if (tile.getBuildings() >= 5) {
        addToast("Properti ini sudah maksimal.", RED);
        return;
    }

    const int cost = tile.getBuildings() >= 4 ? tile.getHotelCost() : tile.getHouseCost();
    if (player.getMoney() < cost) {
        addToast("Saldo tidak cukup untuk membangun.", RED);
        return;
    }

    player.setMoney(player.getMoney() - cost);
    tile.setBuildings(tile.getBuildings() + 1);
    addLog(player.getName(), "Bangun", "Menambah bangunan di " + tile.getName() + " seharga " + std::to_string(cost) + ".");
    addToast("Bangunan ditambahkan di " + tile.getName() + ".", tile.getAccent());
    closeOverlay();
}

void GUIGameController::mortgageSelectedTile() {
    GameState& game = appState_.getGame();
    const std::vector<int> options = currentPlayerMortgageOptions();
    if (options.empty() || game.getPlayers().empty()) {
        addToast("Tidak ada aset yang bisa digadai.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));

    int cashIn = getMortgageValue(tile);
    if (tile.getKind() == TileKind::Street && tile.getBuildings() > 0) {
        cashIn += recoveredBuildingValue(tile);
        tile.setBuildings(0);
    }

    tile.setMortgaged(true);
    player.setMoney(player.getMoney() + cashIn);
    addLog(player.getName(), "Mortgage", "Menggadaikan " + tile.getName() + " dan menerima " + std::to_string(cashIn) + ".");
    addToast("Aset digadai.", tile.getAccent());
    closeOverlay();
}

void GUIGameController::redeemSelectedTile() {
    GameState& game = appState_.getGame();
    const std::vector<int> options = currentPlayerRedeemOptions();
    if (options.empty() || game.getPlayers().empty()) {
        addToast("Tidak ada aset tergadai untuk ditebus.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(options.size()) - 1));
    TileInfo& tile = game.getBoard().at(options.at(selected));
    const int cost = getRedeemCost(tile);
    if (player.getMoney() < cost) {
        addToast("Saldo tidak cukup untuk menebus aset.", RED);
        return;
    }

    player.setMoney(player.getMoney() - cost);
    tile.setMortgaged(false);
    addLog(player.getName(), "Redeem", "Menebus " + tile.getName() + " dengan biaya " + std::to_string(cost) + ".");
    addToast("Aset berhasil ditebus.", tile.getAccent());
    closeOverlay();
}

void GUIGameController::saveSession() {
    GameState& game = appState_.getGame();
    if (!canSaveNow()) {
        addToast("Game hanya bisa disimpan di awal turn.", RED);
        return;
    }

    std::string name = appState_.getSaveInput();
    if (name.empty()) {
        name = "save-" + std::to_string(game.getTurn());
    }

    SaveSlot slot;
    slot.setName(name);
    slot.setSubtitle(
        game.getPlayers().empty()
            ? "Belum ada pemain"
            : game.getPlayers().at(game.getCurrentPlayer()).getName() + " - " +
                  std::to_string(game.getPlayers().at(game.getCurrentPlayer()).getMoney())
    );
    slot.setTurn(game.getTurn());
    slot.setPlayerCount(static_cast<int>(game.getPlayers().size()));
    slot.setAccent(
        game.getPlayers().empty() ? SKYBLUE : game.getPlayers().at(game.getCurrentPlayer()).getAccent()
    );

    std::vector<SaveSlot>& saveSlots = appState_.getSaveSlots();
    int slotIndex = appState_.getSelectedSave();
    if (slotIndex < 0 || slotIndex >= static_cast<int>(saveSlots.size())) {
        saveSlots.push_back(slot);
        appState_.setSelectedSave(static_cast<int>(saveSlots.size()) - 1);
    } else {
        saveSlots.at(slotIndex) = slot;
    }

    game.setSessionLabel(name);
    addLog(
        game.getPlayers().at(game.getCurrentPlayer()).getName(),
        "Save",
        "Menyimpan sesi sebagai " + name + "."
    );
    addToast("Session tersimpan sebagai " + name + ".", SKYBLUE);
    closeOverlay();
}

void GUIGameController::dropSelectedHandCard() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.getHandCards().empty()) {
        closeOverlay();
        return;
    }

    const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(player.getHandCards().size()) - 1));
    const CardInfo discarded = player.getHandCards().at(selected);
    player.getHandCards().erase(player.getHandCards().begin() + selected);
    addLog(player.getName(), "Drop Card", "Membuang kartu " + discarded.getTitle() + ".");
    addToast("Kartu dibuang.", discarded.getAccent());
    if (player.getHandCards().size() <= 3U) {
        closeOverlay();
    } else {
        appState_.getOverlay().setSelectedIndex(std::min(selected, static_cast<int>(player.getHandCards().size()) - 1));
    }
}

void GUIGameController::liquidateSelectedTile() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.getProperties().empty()) {
        addToast("Tidak ada aset untuk dijual.", RED);
        return;
    }

    const int selected = std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(player.getProperties().size()) - 1));
    const int tileIndex = player.getProperties().at(selected);
    TileInfo& tile = game.getBoard().at(tileIndex);
    const int saleValue = getMortgageValue(tile) + recoveredBuildingValue(tile);
    player.setMoney(player.getMoney() + saleValue);
    addLog(player.getName(), "Liquidation", "Menjual " + tile.getName() + " dengan nilai " + std::to_string(saleValue) + ".");
    addToast("Aset dijual untuk menambah cash.", tile.getAccent());
    releaseTile(game, tileIndex);

    if (player.getMoney() >= 0 || player.getProperties().empty()) {
        closeOverlay();
    } else {
        appState_.getOverlay().setSelectedIndex(
            std::max(0, std::min(appState_.getOverlay().getSelectedIndex(), static_cast<int>(player.getProperties().size()) - 1))
        );
    }
}

void GUIGameController::declareBankrupt() {
    GameState& game = appState_.getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    std::vector<int> ownedTiles = player.getProperties();
    for (int tileIndex : ownedTiles) {
        releaseTile(game, tileIndex);
    }
    player.getHandCards().clear();
    player.setJailed(false);
    player.setFailedJailRolls(0);
    player.setBankrupt(true);
    addLog(player.getName(), "Bankrupt", "Pemain dinyatakan bangkrut.");
    addToast(player.getName() + " bangkrut.", RED);
    closeOverlay();

    int activePlayers = 0;
    for (const PlayerInfo& candidate : game.getPlayers()) {
        if (!candidate.isBankrupt()) {
            activePlayers++;
        }
    }
    if (activePlayers <= 1) {
        openGameOver();
        return;
    }

    endTurn();
}

std::vector<int> GUIGameController::currentPlayerStreetOptions() const {
    std::vector<int> streets;
    const GameState& game = appState_.getGame();
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

std::vector<int> GUIGameController::currentPlayerBuildOptions() const {
    std::vector<int> buildable;
    const GameState& game = appState_.getGame();
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

std::vector<int> GUIGameController::currentPlayerMortgageOptions() const {
    std::vector<int> items;
    const GameState& game = appState_.getGame();
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

std::vector<int> GUIGameController::currentPlayerRedeemOptions() const {
    std::vector<int> items;
    const GameState& game = appState_.getGame();
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
