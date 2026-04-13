#include "model/Game.hpp"

using namespace std;

Game::Game(): currentTurn(0), maxTurn(100), isRunning(false) {}

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
) : board(board),
    players(players),
    dice(dice),
    turnManager(turnManager),
    movementManager(movementManager),
    propertyManager(propertyManager),
    auctionManager(auctionManager),
    cardManager(cardManager),
    jailManager(jailManager),
    bankruptcyManager(bankruptcyManager),
    saveLoadManager(saveLoadManager),
    configManager(configManager),
    logManager(logManager),
    taxManager(taxManager),
    festivalManager(festivalManager),
    winConditionManager(winConditionManager),
    currentTurn(currentTurn),
    maxTurn(maxTurn),
    isRunning(isRunning)
{}

Game::Game(const Game& other) {
    board=other.board;
    players=other.players;
    dice=other.dice;
    turnManager=other.turnManager;
    movementManager=other.movementManager;
    propertyManager=other.propertyManager;
    auctionManager=other.auctionManager;
    cardManager=other.cardManager;
    jailManager=other.jailManager;
    bankruptcyManager=other.bankruptcyManager;
    saveLoadManager=other.saveLoadManager;
    configManager=other.configManager;
    logManager=other.logManager;
    taxManager=other.taxManager;
    festivalManager=other.festivalManager;
    winConditionManager=other.winConditionManager;
    currentTurn=other.currentTurn;
    maxTurn=other.maxTurn;
    isRunning=other.isRunning;
}

Game::~Game() {}

Game& Game::operator=(const Game& other) {
    if (this!=&other){
        board=other.board;
        players=other.players;
        dice=other.dice;
        turnManager=other.turnManager;
        movementManager=other.movementManager;
        propertyManager=other.propertyManager;
        auctionManager=other.auctionManager;
        cardManager=other.cardManager;
        jailManager=other.jailManager;
        bankruptcyManager=other.bankruptcyManager;
        saveLoadManager=other.saveLoadManager;
        configManager=other.configManager;
        logManager=other.logManager;
        taxManager=other.taxManager;
        festivalManager=other.festivalManager;
        winConditionManager=other.winConditionManager;
        currentTurn=other.currentTurn;
        maxTurn=other.maxTurn;
        isRunning=other.isRunning;
    }
    return *this;
}

void Game::startNewGame() {
    currentTurn=1;
    isRunning=false;
    bankruptcyManager=BankruptcyManager();
    dice=Dice();

    configManager.loadAllConfigs();
    board.initializeBoard();
    cardManager.initializeDecks();

    if (players.empty()) {
        throw InvalidActionException("Belum ada pemain");
    }

    turnManager.initializeTurnOrder(static_cast<int>(players.size()));

    isRunning=true;
}

void Game::loadGame(const string& filename) {
    saveLoadManager.loadGame(*this, filename);

    if (players.empty()) {
        throw FileException("Tidak ada pemain.");
    }

    isRunning=true;   
}

Player& Game::getCurrentPlayer() {
    if (players.empty()) {
        throw InvalidActionException("Tidak ada pemain");
    }

    if (currentTurn <= 0) {
        return players.at(0);
    }

    const int currentIndex=(currentTurn-1)%static_cast<int>(players.size());
    return players.at(currentIndex);
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

bool Game::isGameOver() {
    return getBankruptcyManager().isBankruptcyActive() || currentTurn >= maxTurn;
}
