#include "model/Game.hpp"

#include "model/NimonException.hpp"

using namespace std;

Game::Game() : gameContext() {}

Game::Game(
    const GameContext& gameContext,
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
    const WinConditionManager& winConditionManager
) : gameContext(gameContext),
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
    winConditionManager(winConditionManager) {}

Game::Game(const Game& other) {
    gameContext = other.gameContext;
    dice = other.dice;
    turnManager = other.turnManager;
    movementManager = other.movementManager;
    propertyManager = other.propertyManager;
    auctionManager = other.auctionManager;
    cardManager = other.cardManager;
    jailManager = other.jailManager;
    bankruptcyManager = other.bankruptcyManager;
    saveLoadManager = other.saveLoadManager;
    configManager = other.configManager;
    logManager = other.logManager;
    taxManager = other.taxManager;
    festivalManager = other.festivalManager;
    winConditionManager = other.winConditionManager;
}

Game::~Game() {}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        gameContext = other.gameContext;
        dice = other.dice;
        turnManager = other.turnManager;
        movementManager = other.movementManager;
        propertyManager = other.propertyManager;
        auctionManager = other.auctionManager;
        cardManager = other.cardManager;
        jailManager = other.jailManager;
        bankruptcyManager = other.bankruptcyManager;
        saveLoadManager = other.saveLoadManager;
        configManager = other.configManager;
        logManager = other.logManager;
        taxManager = other.taxManager;
        festivalManager = other.festivalManager;
        winConditionManager = other.winConditionManager;
    }
    return *this;
}

void Game::startNewGame() {
    gameContext.setCurrentTurn(1);
    gameContext.setIsRunning(false);
    bankruptcyManager = BankruptcyManager();
    dice = Dice();

    configManager.loadAllConfigs();
    if (configManager.getMaxTurn() > 0) {
        gameContext.setMaxTurn(configManager.getMaxTurn());
    }
    gameContext.getBoard().initializeBoard(configManager);
    cardManager.initializeDecks();

    if (gameContext.getPlayers().empty()) {
        throw InvalidActionException("Belum ada pemain");
    }

    turnManager.initializeTurnOrder(static_cast<int>(gameContext.getPlayers().size()));

    gameContext.setIsRunning(true);
}

void Game::loadGame(const string& filename) {
    if (gameContext.isGameRunning()) {
        throw InvalidActionException("Load hanya dapat dilakukan sebelum permainan dimulai.");
    }

    saveLoadManager.loadGame(*this, filename);

    if (gameContext.getPlayers().empty()) {
        throw FileException("Tidak ada pemain.");
    }

    gameContext.setIsRunning(true);
}

Player& Game::getCurrentPlayer() {
    vector<Player>& players = gameContext.getPlayers();
    if (players.empty()) {
        throw InvalidActionException("Tidak ada pemain");
    }

    if (gameContext.getCurrentTurn() <= 0) {
        return players.at(0);
    }

    const int currentIndex = (gameContext.getCurrentTurn() - 1) % static_cast<int>(players.size());
    return players.at(currentIndex);
}

const Player& Game::getCurrentPlayer() const {
    const vector<Player>& players = gameContext.getPlayers();
    if (players.empty()) {
        throw InvalidActionException("Tidak ada pemain");
    }

    if (gameContext.getCurrentTurn() <= 0) {
        return players.at(0);
    }

    const int currentIndex = (gameContext.getCurrentTurn() - 1) % static_cast<int>(players.size());
    return players.at(currentIndex);
}

Board& Game::getBoard() {
    return gameContext.getBoard();
}

const Board& Game::getBoard() const {
    return gameContext.getBoard();
}

GameContext& Game::getGameContext() {
    return gameContext;
}

const GameContext& Game::getGameContext() const {
    return gameContext;
}

int Game::getCurrentTurn() const {
    return gameContext.getCurrentTurn();
}

int Game::getMaxTurn() const {
    return gameContext.getMaxTurn();
}

bool Game::isGameRunning() const {
    return gameContext.isGameRunning();
}

void Game::setCurrentTurn(int currentTurn) {
    gameContext.setCurrentTurn(currentTurn);
}

void Game::setGameRunning(bool isRunning) {
    gameContext.setIsRunning(isRunning);
}

TurnManager& Game::getTurnManager() {
    return turnManager;
}

const TurnManager& Game::getTurnManager() const {
    return turnManager;
}

MovementManager& Game::getMovementManager() {
    return movementManager;
}

const MovementManager& Game::getMovementManager() const {
    return movementManager;
}

PropertyManager& Game::getPropertyManager() {
    return propertyManager;
}

const PropertyManager& Game::getPropertyManager() const {
    return propertyManager;
}

AuctionManager& Game::getAuctionManager() {
    return auctionManager;
}

const AuctionManager& Game::getAuctionManager() const {
    return auctionManager;
}

CardManager& Game::getCardManager() {
    return cardManager;
}

const CardManager& Game::getCardManager() const {
    return cardManager;
}

JailManager& Game::getJailManager() {
    return jailManager;
}

const JailManager& Game::getJailManager() const {
    return jailManager;
}

BankruptcyManager& Game::getBankruptcyManager() {
    return bankruptcyManager;
}

const BankruptcyManager& Game::getBankruptcyManager() const {
    return bankruptcyManager;
}

SaveLoadManager& Game::getSaveLoadManager() {
    return saveLoadManager;
}

const SaveLoadManager& Game::getSaveLoadManager() const {
    return saveLoadManager;
}

ConfigManager& Game::getConfigManager() {
    return configManager;
}

const ConfigManager& Game::getConfigManager() const {
    return configManager;
}

LogManager& Game::getLogManager() {
    return logManager;
}

const LogManager& Game::getLogManager() const {
    return logManager;
}

TaxManager& Game::getTaxManager() {
    return taxManager;
}

const TaxManager& Game::getTaxManager() const {
    return taxManager;
}

FestivalManager& Game::getFestivalManager() {
    return festivalManager;
}

const FestivalManager& Game::getFestivalManager() const {
    return festivalManager;
}

WinConditionManager& Game::getWinConditionManager() {
    return winConditionManager;
}

const WinConditionManager& Game::getWinConditionManager() const {
    return winConditionManager;
}

Dice& Game::getDice() {
    return dice;
}

const Dice& Game::getDice() const {
    return dice;
}

vector<Player>& Game::getPlayers() {
    return gameContext.getPlayers();
}

const vector<Player>& Game::getPlayers() const {
    return gameContext.getPlayers();
}

bool Game::isGameOver() {
    return getBankruptcyManager().isBankruptcyActive() || gameContext.getCurrentTurn() >= gameContext.getMaxTurn();
}

bool Game::isGameOver() const {
    return getBankruptcyManager().isBankruptcyActive() || gameContext.getCurrentTurn() >= gameContext.getMaxTurn();
}
