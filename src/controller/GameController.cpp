#include "controller/GameController.hpp"

using namespace std;

GameController::GameController(Game& game, UIManager& uiManager)
    : game(game), uiManager(uiManager), commandController(nullptr), tileController(nullptr) {}

GameController::GameController(const GameController& other)
    : game(other.game),
      uiManager(other.uiManager),
      commandController(other.commandController),
      tileController(other.tileController) {}

GameController::~GameController() {}

GameController& GameController::operator=(const GameController& other) {
    if (this != &other) {
        commandController = other.commandController;
        tileController = other.tileController;
    }
    return *this;
}

void GameController::runGameLoop() {}

void GameController::runTurn() {}

void GameController::handleStartTurn() {}

void GameController::handleEndTurn() {}

bool GameController::canSaveNow() const {
    return false;
}

bool GameController::isCommandValidThisTurn(const string& input) const {
    return false;
}