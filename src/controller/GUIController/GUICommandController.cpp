#include "controller/GUIController/GUICommandController.hpp"

#include "model/Dice.hpp"
#include "model/Game.hpp"
#include "model/cards/HandCard.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "model/tiles/Tile.hpp"
#include "view/raylib/UiToolkit.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <memory>
#include <string>
#include <vector>

using namespace view::raylibgui;

namespace {
UiToolkit commandToolkit;

Color playerAccent(int index) {
    switch (index) {
        case 0: return commandToolkit.theme().getCoral();
        case 1: return commandToolkit.theme().getTeal();
        case 2: return commandToolkit.theme().getGold();
        default: return commandToolkit.theme().getNavy();
    }
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

int backendJailIndex(Game& game) {
    for (const std::shared_ptr<Tile>& tile : game.getBoard().getTiles()) {
        if (tile != nullptr && tile->onLand() == Tile::TileType::Jail) {
            return tile->getIndex();
        }
    }
    return 1;
}

std::string tileCode(Game& game, int position) {
    std::shared_ptr<Tile> tile = game.getBoard().getTile(position);
    return tile != nullptr ? tile->getCode() : std::to_string(position);
}
}

GUICommandController::GUICommandController(GUIControllerContext& controller)
    : controller_(controller) {
    controller_.openOverlayCallback = [this](OverlayType type) { openOverlay(type); };
    controller_.closeOverlayCallback = [this]() { closeOverlay(); };
    controller_.openForceDropCallback = [this]() { openForceDrop(); };
    controller_.openGameOverCallback = [this]() { openGameOver(); };
    controller_.endTurnCallback = [this]() { endTurn(); };
    controller_.finishTurnAfterDiceIfReadyCallback = [this]() { finishTurnAfterDiceIfReady(); };
}

void GUICommandController::tick(float deltaTime) {
    controller_.appState_.setTime(controller_.appState_.getTime() + deltaTime);
    if (controller_.appState_.getOverlay().getType() == OverlayType::None) {
        controller_.appState_.getOverlay().setAnim(0.0f);
    } else {
        controller_.appState_.getOverlay().setAnim(std::min(1.0f, controller_.appState_.getOverlay().getAnim() + deltaTime * 8.0f));
    }
    controller_.updateToasts(deltaTime);
}

void GUICommandController::handleGlobalShortcuts() {
    if (IsKeyPressed(KEY_ESCAPE) && isOverlayOpen()) {
        const OverlayType type = controller_.appState_.getOverlay().getType();
        const bool mandatory = type == OverlayType::IncomeTax ||
                               type == OverlayType::LuxuryTax ||
                               type == OverlayType::Purchase ||
                               type == OverlayType::Auction ||
                               type == OverlayType::CardDraw ||
                               type == OverlayType::ForceDrop ||
                               type == OverlayType::Liquidation ||
                               type == OverlayType::Jail;
        if (!mandatory) {
            closeOverlay();
        }
        return;
    }

    if (controller_.appState_.getScreen() != Screen::Gameplay || isOverlayOpen()) {
        return;
    }

    if (IsKeyPressed(KEY_R)) {
        rollDice();
    } else if (IsKeyPressed(KEY_D)) {
        openSetDice();
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
    controller_.appState_.getOverlay().setType(type);
    controller_.appState_.getOverlay().setAnim(0.0f);
    controller_.appState_.setActiveField(type == OverlayType::Save ? "save-name" : "");
}

void GUICommandController::closeOverlay() {
    controller_.closeCardDrawOverlay(true);
    controller_.appState_.setOverlay(OverlayState{});
    controller_.appState_.getActiveField().clear();
}

bool GUICommandController::isOverlayOpen() const {
    return controller_.appState_.getOverlay().getType() != OverlayType::None;
}

void GUICommandController::openPortfolio() {
    openOverlay(OverlayType::Portfolio);
}

void GUICommandController::openSave() {
    openOverlay(OverlayType::Save);
}

void GUICommandController::openLogs() {
    controller_.syncViewFromBackend();
    const int count = static_cast<int>(controller_.appState_.getGame().getLogs().size());
    controller_.appState_.getOverlay().setSelectedIndex(count);
    openOverlay(OverlayType::Logs);
}

void GUICommandController::openSetDice() {
    controller_.appState_.getOverlay().setManualDieOne(std::max(1, controller_.appState_.getGame().getLastDieOne()));
    controller_.appState_.getOverlay().setManualDieTwo(std::max(1, controller_.appState_.getGame().getLastDieTwo()));
    openOverlay(OverlayType::SetDice);
}

void GUICommandController::openHelp() {
    openOverlay(OverlayType::Help);
}

void GUICommandController::openForceDrop() {
    controller_.appState_.getOverlay().setSelectedIndex(0);
    openOverlay(OverlayType::ForceDrop);
}

void GUICommandController::openGameOver() {
    openOverlay(OverlayType::GameOver);
}

void GUICommandController::startTurn() {
    if (controller_.appState_.getScreen() != Screen::Gameplay ||
        !controller_.backendGame_.isGameRunning() ||
        controller_.backendGame_.getPlayers().empty()) {
        return;
    }
    if (controller_.guiTurnStarted_) {
        controller_.addToast("Turn sudah berjalan.", RED);
        return;
    }

    try {
        controller_.backendGame_.getTurnManager().startTurn(controller_.backendGame_.getGameContext());
        Player& player = controller_.backendGame_.getCurrentPlayer();
        if (player.isBankrupt()) {
            controller_.addToast("Pemain ini sudah bangkrut.", RED);
            controller_.syncViewFromBackend();
            controller_.guiTurnStarted_ = true;
            endTurn();
            return;
        }

        player.setUsedHandCardThisTurn(false);
        controller_.guiTurnStarted_ = true;
        controller_.diceRolledThisTurn_ = false;
        controller_.addLog(player.getUsername(), "START_TURN", "Memulai giliran.");

        std::shared_ptr<HandCard> drawnCard = controller_.backendGame_.getCardManager().giveStartTurnCard(player);
        if (drawnCard != nullptr) {
            controller_.addLog(player.getUsername(), "KARTU", "Mendapat kartu kemampuan " + drawnCard->getName() + ".");
        }
        controller_.syncViewFromBackend();
        controller_.addToast("Giliran " + player.getUsername() + " dimulai.", playerAccent(controller_.currentBackendPlayerIndex()));
        if (drawnCard != nullptr) {
            controller_.addToast("Kartu baru: " + drawnCard->getName() + ".", SKYBLUE);
        }

        if (controller_.backendGame_.getCardManager().needsForceDrop(player)) {
            openForceDrop();
        } else if (player.isJailed()) {
            if (player.getFailedJailRolls() >= 3) {
                const int fine = controller_.backendGame_.getConfigManager().getJailFine();
                if (player.getMoney() < player.effectiveCost(fine)) {
                    controller_.backendGame_.getJailManager().releaseFromJail(player);
                    controller_.backendGame_.getBankruptcyManager().beginBankruptcySession(player, nullptr, fine, true);
                    controller_.addLog(player.getUsername(), "JAIL", "Dana kurang - likuidasi untuk denda wajib jail.");
                    controller_.addToast("Dana kurang untuk denda jail - likuidasi aset.", RED);
                    controller_.syncViewFromBackend();
                    controller_.maybeOpenLiquidation();
                } else {
                    const int actualFine = player.effectiveCost(fine);
                    controller_.backendGame_.getJailManager().payJailFine(player, fine);
                    controller_.backendGame_.getJailManager().releaseFromJail(player);
                    controller_.addLog(player.getUsername(), "JAIL", "Wajib bayar denda setelah 3 kali gagal double sebesar M" + std::to_string(actualFine) + ".");
                    controller_.addToast("Denda jail wajib dibayar M" + std::to_string(actualFine) + ".", SKYBLUE);
                    controller_.syncViewFromBackend();
                }
            } else {
                controller_.openJail();
            }
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUICommandController::rollDice() {
    if (controller_.appState_.getScreen() != Screen::Gameplay ||
        !controller_.backendGame_.isGameRunning() ||
        controller_.backendGame_.getPlayers().empty()) {
        return;
    }
    if (!controller_.guiTurnStarted_) {
        controller_.addToast("Turn belum siap untuk melempar dadu.", RED);
        return;
    }
    if (controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        if (player.isJailed()) {
            controller_.openJail();
            controller_.addToast("Pemain masih di jail.", RED);
            return;
        }

        controller_.backendGame_.getDice().roll();
        controller_.backendGame_.getTurnManager().registerDiceResult(controller_.backendGame_.getDice().isDouble());
        controller_.diceRolledThisTurn_ = true;

        if (controller_.backendGame_.getTurnManager().getConsecutiveDoubles() >= 3) {
            const int previousPosition = player.getPosition();
            controller_.backendGame_.getJailManager().sendToJail(player);
            player.moveTo(backendJailIndex(controller_.backendGame_));
            controller_.addLog(
                player.getUsername(),
                "GERAK",
                "Double tiga kali memindahkan bidak dari " + tileCode(controller_.backendGame_, previousPosition) +
                    " ke " + tileCode(controller_.backendGame_, player.getPosition()) + "."
            );
            if (player.isJailed()) {
                controller_.addLog(player.getUsername(), "JAIL", "Masuk penjara karena double tiga kali berturut-turut.");
            } else {
                controller_.addLog(player.getUsername(), "JAIL", "Mampir ke penjara karena double tiga kali berturut-turut.");
            }
            controller_.syncViewFromBackend();
            finishTurnAfterDiceIfReady();
            return;
        }

        const int total = controller_.backendGame_.getDice().getTotal();
        const int destination = controller_.moveBackendPlayer(player, total);
        controller_.addLog(
            player.getUsername(),
            "DADU",
            "Lempar " + std::to_string(controller_.backendGame_.getDice().getDie1()) + "+" +
                std::to_string(controller_.backendGame_.getDice().getDie2()) + "=" + std::to_string(total)
        );
        controller_.addToast(
            "Dadu " + std::to_string(controller_.backendGame_.getDice().getDie1()) + " + " +
                std::to_string(controller_.backendGame_.getDice().getDie2()),
            playerAccent(controller_.currentBackendPlayerIndex())
        );
        controller_.resolveBackendLanding(destination, false);
        finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUICommandController::applyManualDice() {
    if (controller_.appState_.getScreen() != Screen::Gameplay ||
        !controller_.backendGame_.isGameRunning() ||
        controller_.backendGame_.getPlayers().empty()) {
        return;
    }
    if (!controller_.guiTurnStarted_) {
        controller_.addToast("Turn belum siap untuk mengatur dadu.", RED);
        return;
    }
    if (controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        controller_.addToast("Dadu sudah dilempar pada turn ini.", RED);
        return;
    }

    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        if (player.isJailed()) {
            controller_.openJail();
            controller_.addToast("Pemain masih di jail.", RED);
            return;
        }

        const int dieOne = controller_.appState_.getOverlay().getManualDieOne();
        const int dieTwo = controller_.appState_.getOverlay().getManualDieTwo();
        controller_.backendGame_.getDice() = Dice(dieOne, dieTwo, true);
        controller_.backendGame_.getTurnManager().registerDiceResult(controller_.backendGame_.getDice().isDouble());
        controller_.diceRolledThisTurn_ = true;
        closeOverlay();

        if (controller_.backendGame_.getTurnManager().getConsecutiveDoubles() >= 3) {
            const int previousPosition = player.getPosition();
            controller_.backendGame_.getJailManager().sendToJail(player);
            player.moveTo(backendJailIndex(controller_.backendGame_));
            controller_.addLog(
                player.getUsername(),
                "GERAK",
                "Double tiga kali memindahkan bidak dari " + tileCode(controller_.backendGame_, previousPosition) +
                    " ke " + tileCode(controller_.backendGame_, player.getPosition()) + "."
            );
            if (player.isJailed()) {
                controller_.addLog(player.getUsername(), "JAIL", "Masuk penjara karena double tiga kali berturut-turut.");
            } else {
                controller_.addLog(player.getUsername(), "JAIL", "Mampir ke penjara karena double tiga kali berturut-turut.");
            }
            controller_.syncViewFromBackend();
            finishTurnAfterDiceIfReady();
            return;
        }

        const int destination = controller_.moveBackendPlayer(player, dieOne + dieTwo);
        controller_.addLog(
            player.getUsername(),
            "ATUR_DADU",
            "Dadu manual " + std::to_string(dieOne) + "+" + std::to_string(dieTwo)
        );
        controller_.addToast("Dadu manual diterapkan.", playerAccent(controller_.currentBackendPlayerIndex()));
        controller_.resolveBackendLanding(destination, false);
        finishTurnAfterDiceIfReady();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUICommandController::endTurn() {
    if (!controller_.backendGame_.isGameRunning() || controller_.backendGame_.getPlayers().empty()) {
        return;
    }

    try {
        Player& player = controller_.backendGame_.getCurrentPlayer();
        if (player.getMoney() < 0 || controller_.backendGame_.getBankruptcyManager().isBankruptcyActive()) {
            controller_.syncViewFromBackend();
            controller_.maybeOpenLiquidation();
            return;
        }

        player.setShieldActive(false);
        player.decrementDiscountDuration();
        const int previousTurn = controller_.backendGame_.getCurrentTurn();
        controller_.backendGame_.getTurnManager().endTurn(controller_.backendGame_.getGameContext());
        if (controller_.backendGame_.getCurrentTurn() > previousTurn) {
            std::vector<OwnableTile*> properties;
            for (const std::shared_ptr<Tile>& tile : controller_.backendGame_.getBoard().getTiles()) {
                std::shared_ptr<OwnableTile> ownable = std::dynamic_pointer_cast<OwnableTile>(tile);
                if (ownable != nullptr) {
                    properties.push_back(ownable.get());
                }
            }
            controller_.backendGame_.getFestivalManager().decrementFestivalDurations(properties);
        }

        controller_.guiTurnStarted_ = false;
        controller_.diceRolledThisTurn_ = false;
        controller_.syncViewFromBackend();
        if (controller_.backendGame_.isGameOver()) {
            openGameOver();
        } else {
            startTurn();
        }
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}

void GUICommandController::finishTurnAfterDiceIfReady() {
    if (!controller_.backendGame_.isGameRunning() ||
        controller_.backendGame_.getPlayers().empty() ||
        !controller_.guiTurnStarted_ ||
        !controller_.backendGame_.getTurnManager().isRolledThisTurn()) {
        return;
    }
    if (controller_.appState_.getOverlay().getType() != OverlayType::None) {
        return;
    }
    if (controller_.backendGame_.getBankruptcyManager().isBankruptcyActive() ||
        controller_.backendGame_.getCurrentPlayer().getMoney() < 0) {
        controller_.maybeOpenLiquidation();
        return;
    }

    Player& player = controller_.backendGame_.getCurrentPlayer();
    if (controller_.backendGame_.getDice().isDouble() &&
        controller_.backendGame_.getTurnManager().getConsecutiveDoubles() > 0 &&
        !player.isBankrupt() &&
        !player.isJailed() &&
        controller_.backendGame_.getTurnManager().getConsecutiveDoubles() < 3) {
        controller_.backendGame_.getTurnManager().setRolledThisTurn(false);
        controller_.diceRolledThisTurn_ = false;
        controller_.addLog(player.getUsername(), "DADU", "Dadu double: dapat kesempatan melempar lagi.");
        controller_.addToast("Dadu double! Lempar lagi.", playerAccent(controller_.currentBackendPlayerIndex()));
        controller_.syncViewFromBackend();
        return;
    }

    endTurn();
}

bool GUICommandController::canSaveNow() const {
    return controller_.backendGame_.isGameRunning() &&
        controller_.guiTurnStarted_ &&
        !controller_.backendGame_.getTurnManager().hasActionTakenThisTurn() &&
        (controller_.appState_.getOverlay().getType() == OverlayType::None ||
         controller_.appState_.getOverlay().getType() == OverlayType::Save);
}

void GUICommandController::adjustManualDie(int dieIndex, int delta) {
    OverlayState& overlay = controller_.appState_.getOverlay();
    const int die = dieIndex == 1 ? overlay.getManualDieOne() : overlay.getManualDieTwo();
    const int adjusted = std::max(1, std::min(6, die + delta));
    if (dieIndex == 1) {
        overlay.setManualDieOne(adjusted);
    } else {
        overlay.setManualDieTwo(adjusted);
    }
}

void GUICommandController::saveSession() {
    if (!canSaveNow()) {
        controller_.addToast("Game hanya bisa disimpan di awal turn.", RED);
        return;
    }

    std::string filename = withTxtExtension(controller_.appState_.getSaveInput());
    try {
        controller_.backendGame_.getSaveLoadManager().saveGame(controller_.backendGame_, filename);

        SaveSlot slot;
        slot.setName(filename);
        slot.setSubtitle(
            controller_.backendGame_.getPlayers().empty()
                ? "Belum ada pemain"
                : controller_.backendGame_.getCurrentPlayer().getUsername() + " - " +
                      std::to_string(controller_.backendGame_.getCurrentPlayer().getMoney())
        );
        slot.setTurn(controller_.backendGame_.getCurrentTurn());
        slot.setPlayerCount(static_cast<int>(controller_.backendGame_.getPlayers().size()));
        slot.setAccent(playerAccent(controller_.currentBackendPlayerIndex()));

        std::vector<SaveSlot>& saveSlots = controller_.appState_.getSaveSlots();
        int slotIndex = controller_.appState_.getSelectedSave();
        if (slotIndex < 0 || slotIndex >= static_cast<int>(saveSlots.size())) {
            saveSlots.push_back(slot);
            controller_.appState_.setSelectedSave(static_cast<int>(saveSlots.size()) - 1);
        } else {
            saveSlots.at(slotIndex) = slot;
        }

        controller_.addToast("Session tersimpan sebagai " + filename + ".", SKYBLUE);
        closeOverlay();
        controller_.syncViewFromBackend();
    } catch (const std::exception& exception) {
        controller_.addToast(exception.what(), RED);
        controller_.syncViewFromBackend();
    }
}
