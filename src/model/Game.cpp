#include "model/Game.hpp"

using namespace std;

Game::Game() {}

Game::Game(
    const Board& board,
    const vector<Player>& players,
    const Dice& dice,
    const TurnManager& turnManager,
    const MovementManager& movementManager,
    const PropertyManager& propertyManager,
    const AuctionManager& auctionManager,
    const CardManager& cardManager,
    const JailManager& jailManager,
    const BankruptcyManager& bankruptcyManager,
    const SaveLoadManager& saveLoadManager,
    const ConfigManager& configManager,
    const LogManager& logManager,
    const TaxManager& taxManager,
    const FestivalManager& festivalManager,
    const WinConditionManager& winConditionManager,
    int currentTurn,
    int maxTurn,
    bool isRunning
) {}

Game::Game(const Game& other) {}

Game::~Game() {}

Game& Game::operator=(const Game& other) {
    return *this;
}

void Game::startNewGame() {}

void Game::loadGame(const string& filename) {}

Player& Game::getCurrentPlayer() {
    return players.at(0);
}

Board& Game::getBoard() {
    return board;
}

TurnManager& Game::getTurnManager() {
    return turnManager;
}

MovementManager& Game::getMovementManager() {
    return movementManager;
}

PropertyManager& Game::getPropertyManager() {
    return propertyManager;
}

AuctionManager& Game::getAuctionManager() {
    return auctionManager;
}

CardManager& Game::getCardManager() {
    return cardManager;
}

JailManager& Game::getJailManager() {
    return jailManager;
}

BankruptcyManager& Game::getBankruptcyManager() {
    return bankruptcyManager;
}

SaveLoadManager& Game::getSaveLoadManager() {
    return saveLoadManager;
}

ConfigManager& Game::getConfigManager() {
    return configManager;
}

LogManager& Game::getLogManager() {
    return logManager;
}

TaxManager& Game::getTaxManager() {
    return taxManager;
}

FestivalManager& Game::getFestivalManager() {
    return festivalManager;
}

WinConditionManager& Game::getWinConditionManager() {
    return winConditionManager;
}

Dice& Game::getDice() {
    return dice;
}

vector<Player>& Game::getPlayers() {
    return players;
}

bool Game::isGameOver() const {
    return false;
}