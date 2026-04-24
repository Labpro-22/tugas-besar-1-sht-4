#include "controller/GUIController/GUICommandController.hpp"

#include "controller/GUIController/GUIGameController.hpp"

#include <algorithm>
#include <random>

using namespace view::raylibgui;

GUICommandController::GUICommandController(GUIGameController& controller)
    : controller_(controller) {}

void GUICommandController::tick(float deltaTime) {
    AppState& state = controller_.state();
    state.setTime(state.getTime() + deltaTime);
    if (state.getOverlay().getType() == OverlayType::None) {
        state.getOverlay().setAnim(0.0f);
        return;
    }

    state.getOverlay().setAnim(std::min(1.0f, state.getOverlay().getAnim() + deltaTime * 8.0f));
}

void GUICommandController::handleGlobalShortcuts() {
    AppState& state = controller_.state();
    if (IsKeyPressed(KEY_ESCAPE) && isOverlayOpen()) {
        closeOverlay();
        return;
    }

    if (state.getScreen() != Screen::Gameplay || isOverlayOpen()) {
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
        controller_.openCards();
    } else if (IsKeyPressed(KEY_L)) {
        openLogs();
    } else if (IsKeyPressed(KEY_H)) {
        openHelp();
    }
}

void GUICommandController::openOverlay(OverlayType type) {
    AppState& state = controller_.state();
    state.getOverlay().setType(type);
    state.getOverlay().setAnim(0.0f);
    state.setActiveField(type == OverlayType::Save ? "save-name" : "");
}

void GUICommandController::closeOverlay() {
    AppState& state = controller_.state();
    state.setOverlay(OverlayState{});
    state.getActiveField().clear();
}

bool GUICommandController::isOverlayOpen() const {
    return controller_.state().getOverlay().getType() != OverlayType::None;
}

void GUICommandController::openPortfolio() {
    openOverlay(OverlayType::Portfolio);
}

void GUICommandController::openSave() {
    openOverlay(OverlayType::Save);
}

void GUICommandController::openLogs() {
    openOverlay(OverlayType::Logs);
}

void GUICommandController::openSetDice() {
    AppState& state = controller_.state();
    state.getOverlay().setManualDieOne(std::max(1, state.getGame().getLastDieOne()));
    state.getOverlay().setManualDieTwo(std::max(1, state.getGame().getLastDieTwo()));
    openOverlay(OverlayType::SetDice);
}

void GUICommandController::openHelp() {
    openOverlay(OverlayType::Help);
}

void GUICommandController::openForceDrop() {
    controller_.state().getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::ForceDrop);
}

void GUICommandController::openGameOver() {
    openOverlay(OverlayType::GameOver);
}

void GUICommandController::startTurn() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    if (state.getScreen() != Screen::Gameplay || game.getPlayers().empty()) {
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.isBankrupt()) {
        controller_.addToast("Pemain ini sudah bangkrut.", RED);
        return;
    }

    game.setTurnStarted(true);
    game.setRolledThisTurn(false);
    controller_.addLog(player.getName(), "Start Turn", "Memulai giliran.");

    if (player.isJailed()) {
        controller_.openJail();
    } else if (player.getHandCards().size() > 3U) {
        controller_.openForceDrop();
    }
}

void GUICommandController::rollDice() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    if (state.getScreen() != Screen::Gameplay || game.getPlayers().empty()) {
        return;
    }

    if (!game.isTurnStarted()) {
        controller_.addToast("Mulai turn dulu sebelum melempar dadu.", RED);
        return;
    }
    if (game.isRolledThisTurn()) {
        controller_.addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.isJailed()) {
        controller_.openJail();
        controller_.addToast("Pemain masih di jail.", RED);
        return;
    }

    std::uniform_int_distribution<int> distribution(1, 6);
    game.setLastDieOne(distribution(state.getRng()));
    game.setLastDieTwo(distribution(state.getRng()));
    game.setRolledThisTurn(true);

    const int steps = game.getLastDieOne() + game.getLastDieTwo();
    const int previousPosition = player.getPosition();
    const int boardSize = static_cast<int>(game.getBoard().size());
    const int nextPosition = boardSize > 0 ? (previousPosition + steps) % boardSize : previousPosition;
    if (boardSize > 0 && previousPosition + steps >= boardSize) {
        player.setMoney(player.getMoney() + 200);
        controller_.addToast("Melewati GO: +200.", GOLD);
    }

    player.setPosition(nextPosition);
    controller_.addLog(
        player.getName(),
        "Roll Dice",
        "Melempar " + std::to_string(game.getLastDieOne()) + " dan " + std::to_string(game.getLastDieTwo()) +
            ", mendarat di " + game.getBoard().at(nextPosition).getName() + "."
    );
    controller_.addToast("Dadu " + std::to_string(game.getLastDieOne()) + " + " + std::to_string(game.getLastDieTwo()), player.getAccent());
    controller_.triggerTileEvent(nextPosition, false);
}

void GUICommandController::applyManualDice() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    if (state.getScreen() != Screen::Gameplay || game.getPlayers().empty()) {
        return;
    }

    if (!game.isTurnStarted()) {
        controller_.addToast("Mulai turn dulu sebelum mengatur dadu.", RED);
        return;
    }
    if (game.isRolledThisTurn()) {
        controller_.addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    PlayerInfo& player = game.getPlayers().at(game.getCurrentPlayer());
    if (player.isJailed()) {
        controller_.openJail();
        controller_.addToast("Pemain masih di jail.", RED);
        return;
    }

    const OverlayState& overlay = state.getOverlay();
    game.setLastDieOne(overlay.getManualDieOne());
    game.setLastDieTwo(overlay.getManualDieTwo());
    game.setRolledThisTurn(true);
    controller_.closeOverlay();

    const int steps = game.getLastDieOne() + game.getLastDieTwo();
    const int previousPosition = player.getPosition();
    const int boardSize = static_cast<int>(game.getBoard().size());
    const int nextPosition = boardSize > 0 ? (previousPosition + steps) % boardSize : previousPosition;
    if (boardSize > 0 && previousPosition + steps >= boardSize) {
        player.setMoney(player.getMoney() + 200);
        controller_.addToast("Melewati GO: +200.", GOLD);
    }

    player.setPosition(nextPosition);
    controller_.addLog(
        player.getName(),
        "Set Dice",
        "Menggunakan dadu manual " + std::to_string(game.getLastDieOne()) + " dan " + std::to_string(game.getLastDieTwo()) +
            ", mendarat di " + game.getBoard().at(nextPosition).getName() + "."
    );
    controller_.addToast("Dadu manual diterapkan.", player.getAccent());
    controller_.triggerTileEvent(nextPosition, false);
}

void GUICommandController::endTurn() {
    GameState& game = controller_.state().getGame();
    if (game.getPlayers().empty()) {
        return;
    }

    if (game.getPlayers().at(game.getCurrentPlayer()).getMoney() < 0) {
        controller_.maybeOpenLiquidation();
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
        controller_.openGameOver();
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
        controller_.openGameOver();
    }
}

bool GUICommandController::canSaveNow() const {
    const GameState& game = controller_.state().getGame();
    return game.isTurnStarted() && !game.isRolledThisTurn();
}

void GUICommandController::adjustManualDie(int dieIndex, int delta) {
    OverlayState& overlay = controller_.state().getOverlay();
    const int die = dieIndex == 1 ? overlay.getManualDieOne() : overlay.getManualDieTwo();
    const int adjusted = std::max(1, std::min(6, die + delta));
    if (dieIndex == 1) {
        overlay.setManualDieOne(adjusted);
        return;
    }

    overlay.setManualDieTwo(adjusted);
}

void GUICommandController::saveSession() {
    AppState& state = controller_.state();
    GameState& game = state.getGame();
    if (!canSaveNow()) {
        controller_.addToast("Game hanya bisa disimpan di awal turn.", RED);
        return;
    }

    std::string name = state.getSaveInput();
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

    std::vector<SaveSlot>& saveSlots = state.getSaveSlots();
    int slotIndex = state.getSelectedSave();
    if (slotIndex < 0 || slotIndex >= static_cast<int>(saveSlots.size())) {
        saveSlots.push_back(slot);
        state.setSelectedSave(static_cast<int>(saveSlots.size()) - 1);
    } else {
        saveSlots.at(slotIndex) = slot;
    }

    game.setSessionLabel(name);
    controller_.addLog(
        game.getPlayers().at(game.getCurrentPlayer()).getName(),
        "Save",
        "Menyimpan sesi sebagai " + name + "."
    );
    controller_.addToast("Session tersimpan sebagai " + name + ".", SKYBLUE);
    controller_.closeOverlay();
}
