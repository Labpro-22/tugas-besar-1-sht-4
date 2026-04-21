#include "controller/CommandController.hpp"

using namespace std;

CommandController::CommandController(Game& game, UIManager& uiManager)
    : game(game), uiManager(uiManager) {}

CommandController::CommandController(const CommandController& other)
    : game(other.game), uiManager(other.uiManager) {}

CommandController::~CommandController() {}

CommandController& CommandController::operator=(const CommandController& other) {
    return *this;
}

void CommandController::processCommand(const string& input) {}

void CommandController::handleRollDice() {}

void CommandController::handleSetDice(int x, int y) {}

void CommandController::handlePrintBoard() {}

void CommandController::handlePrintDeed() {}

void CommandController::handlePrintProperties() {}

void CommandController::handleMortgage() {}

void CommandController::handleRedeem() {}

void CommandController::handleBuild() {}

void CommandController::handleSave(const string& filename) {}

void CommandController::handlePrintLog(int recentCount) {}

void CommandController::handleUseCard() {}

int CommandController::parseRecentLogCount(const string& input) const {
    return 0;
}