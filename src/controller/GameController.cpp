#include "controller/GameController.hpp"

#include "controller/CommandController.hpp"
#include "controller/TileController.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/managers/PropertyManager.hpp"
#include "model/managers/WinConditionManager.hpp"
#include "model/tiles/OwnableTile.hpp"
#include "view/UIManager.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

string GameController::toUpperCopy(string value) const {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(toupper(ch));
    });
    return value;
}

string GameController::firstToken(const string& input) const {
    istringstream stream(input);
    string command;
    stream >> command;
    return toUpperCopy(command);
}

bool GameController::isTurnEndingCommand(const string& input) const {
    const string command = firstToken(input);
    return command == "LEMPAR_DADU" ||
           command == "ATUR_DADU";
}

size_t GameController::activePlayerCount(const Game& game) const {
    size_t count = 0;
    for (const Player& player : game.getPlayers()) {
        if (!player.isBankrupt()) {
            count++;
        }
    }
    return count;
}

GameController::GameController(Game& game, UIManager& uiManager)
    : game(game),
      uiManager(uiManager),
      commandController(make_unique<CommandController>(game, uiManager)),
      tileController(make_unique<TileController>(game, uiManager)),
      rolledThisTurn(false),
      diceRolledThisTurn(false),
      resumeLoadedTurnPending(false) {}

GameController::GameController(const GameController& other)
    : game(other.game),
      uiManager(other.uiManager),
      commandController(make_unique<CommandController>(game, uiManager)),
      tileController(make_unique<TileController>(game, uiManager)),
      rolledThisTurn(other.rolledThisTurn),
      diceRolledThisTurn(other.diceRolledThisTurn),
      resumeLoadedTurnPending(other.resumeLoadedTurnPending) {}

GameController::~GameController() = default;

GameController& GameController::operator=(const GameController& other) {
    if (this != &other) {
        commandController = make_unique<CommandController>(game, uiManager);
        tileController = make_unique<TileController>(game, uiManager);
        rolledThisTurn = other.rolledThisTurn;
        diceRolledThisTurn = other.diceRolledThisTurn;
        resumeLoadedTurnPending = other.resumeLoadedTurnPending;
    }
    return *this;
}

void GameController::runGameLoop() {
    if (commandController == nullptr) {
        commandController = make_unique<CommandController>(game, uiManager);
    }
    if (tileController == nullptr) {
        tileController = make_unique<TileController>(game, uiManager);
    }

    while (game.isGameRunning()) {
        if (activePlayerCount(game) <= 1 || game.getCurrentTurn() > game.getMaxTurn()) {
            break;
        }

        runTurn();
    }

    const bool endedByBankruptcy = activePlayerCount(game) <= 1;
    const bool endedByMaxTurn = game.getCurrentTurn() > game.getMaxTurn();
    if (!endedByBankruptcy && !endedByMaxTurn) {
        return;
    }

    vector<Player*> winners = game.getWinConditionManager().determineWinners(game);
    vector<string> winnerNames;
    vector<int> winnerMoney;
    vector<int> winnerPropertyCounts;
    vector<int> winnerCardCounts;

    for (Player* winner : winners) {
        if (winner == nullptr) {
            continue;
        }
        winnerNames.push_back(winner->getUsername());
        winnerMoney.push_back(winner->getMoney());
        winnerPropertyCounts.push_back(
            game.getPropertyManager().countOwnedProperties(game.getBoard(), *winner)
        );
        winnerCardCounts.push_back(winner->countCards());
    }

    uiManager.printGameOver(
        winnerNames,
        winnerMoney,
        winnerPropertyCounts,
        winnerCardCounts,
        game.getCurrentTurn() > game.getMaxTurn()
    );
}

void GameController::runTurn() {
    const bool resumeExistingTurn = resumeLoadedTurnPending;
    resumeLoadedTurnPending = false;
    handleStartTurn(resumeExistingTurn);

    Player& player = game.getCurrentPlayer();
    if (player.isBankrupt()) {
        handleEndTurn();
        return;
    }

    while (game.isGameRunning() && !rolledThisTurn) {
        string input = uiManager.readCommand();
        if (input.empty()) {
            if (cin.eof()) {
                game.setGameRunning(false);
                return;
            }
            continue;
        }

        if (!isCommandValidThisTurn(input)) {
            uiManager.printError("Perintah tidak valid untuk kondisi giliran saat ini.");
            continue;
        }

        const bool commandSucceeded = commandController->processCommand(input);

        if (commandSucceeded && isTurnEndingCommand(input)) {
            diceRolledThisTurn = true;
            if (game.getTurnManager().getConsecutiveDoubles() >= 3) {
                rolledThisTurn = true;
                continue;
            }

            if (game.getDice().isDouble() && game.isGameRunning() && !player.isBankrupt() && !player.isJailed()) {
                uiManager.printMessage("Dadu double! Kamu mendapat kesempatan melempar lagi.");
                rolledThisTurn = false;
                continue;
            }
            rolledThisTurn = true;
        }
    }

    handleEndTurn();
}

void GameController::resumeLoadedTurn() {
    resumeLoadedTurnPending = true;
}

void GameController::handleStartTurn(bool resumeExistingTurn) {
    rolledThisTurn = false;
    diceRolledThisTurn = false;
    if (!resumeExistingTurn) {
        game.getTurnManager().startTurn(game.getGameContext());
    } else {
        game.getTurnManager().setRolledThisTurn(false);
    }

    Player& player = game.getCurrentPlayer();
    if (player.isBankrupt()) {
        return;
    }

    if (!resumeExistingTurn) {
        player.setUsedHandCardThisTurn(false);

        vector<OwnableTile*> ownedProperties = game.getPropertyManager().getOwnedProperties(game.getBoard(), player);
        game.getFestivalManager().decrementFestivalDurations(ownedProperties);
    }

    uiManager.printMessage("");
    uiManager.printMessage("=================================");
    uiManager.printMessage(
        "Giliran " + player.getUsername() +
        " | Turn " + to_string(game.getCurrentTurn()) +
        "/" + to_string(game.getMaxTurn())
    );
    uiManager.printMessage("=================================");

    if (resumeExistingTurn) {
        return;
    }

    game.getCardManager().giveStartTurnCard(player);
    if (game.getCardManager().needsForceDrop(player)) {
        tileController->handleForceDrop(player);
    }

    if (player.isJailed()) {
        tileController->handleJailTurn(player);
        if (player.isJailed()) {
            rolledThisTurn = true;
        }
    }
}

void GameController::handleEndTurn() {
    Player& player = game.getCurrentPlayer();
    player.setShieldActive(false);
    player.decrementDiscountDuration();
    game.getTurnManager().endTurn(game.getGameContext());
}

bool GameController::canSaveNow() const {
    return !diceRolledThisTurn;
}

bool GameController::isCommandValidThisTurn(const string& input) const {
    const string command = firstToken(input);
    if (command.empty()) {
        return false;
    }

    if (command == "SIMPAN") {
        return canSaveNow();
    }

    if (command == "MUAT") {
        uiManager.printError("MUAT hanya dapat dilakukan sebelum permainan dimulai melalui menu Load Game.");
        return false;
    }

    if (rolledThisTurn && (command == "LEMPAR_DADU" || command == "ATUR_DADU")) {
        return false;
    }

    return command == "CETAK_PAPAN" ||
           command == "LEMPAR_DADU" ||
           command == "ATUR_DADU" ||
           command == "CETAK_AKTA" ||
           command == "CETAK_PROPERTI" ||
           command == "GADAI" ||
           command == "TEBUS" ||
           command == "BANGUN" ||
           command == "SIMPAN" ||
           command == "CETAK_LOG" ||
           command == "GUNAKAN_KEMAMPUAN" ||
           command == "HELP";
}
