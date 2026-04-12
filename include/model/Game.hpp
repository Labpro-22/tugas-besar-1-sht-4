#pragma once

#include <string>
#include <vector>

#include "model/Board.hpp"
#include "model/Dice.hpp"
#include "model/Player.hpp"
#include "model/managers/TurnManager.hpp"
#include "model/managers/MovementManager.hpp"
#include "model/managers/PropertyManager.hpp"
#include "model/managers/AuctionManager.hpp"
#include "model/managers/CardManager.hpp"
#include "model/managers/JailManager.hpp"
#include "model/managers/BankruptcyManager.hpp"
#include "model/managers/SaveLoadManager.hpp"
#include "model/managers/ConfigManager.hpp"
#include "model/managers/LogManager.hpp"
#include "model/managers/TaxManager.hpp"
#include "model/managers/FestivalManager.hpp"
#include "model/managers/WinConditionManager.hpp"

using namespace std;

class Game {
private:
    Board board;
    vector<Player> players;
    Dice dice;
    TurnManager turnManager;
    MovementManager movementManager;
    PropertyManager propertyManager;
    AuctionManager auctionManager;
    CardManager cardManager;
    JailManager jailManager;
    BankruptcyManager bankruptcyManager;
    SaveLoadManager saveLoadManager;
    ConfigManager configManager;
    LogManager logManager;
    TaxManager taxManager;
    FestivalManager festivalManager;
    WinConditionManager winConditionManager;
    int currentTurn;
    int maxTurn;
    bool isRunning;

public:
    Game();
    Game(
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
    );
    Game(const Game& other);
    ~Game();
    Game& operator=(const Game& other);

    void startNewGame();
    void loadGame(const string& filename);

    Player& getCurrentPlayer();
    Board& getBoard();
    TurnManager& getTurnManager();
    MovementManager& getMovementManager();
    PropertyManager& getPropertyManager();
    AuctionManager& getAuctionManager();
    CardManager& getCardManager();
    JailManager& getJailManager();
    BankruptcyManager& getBankruptcyManager();
    SaveLoadManager& getSaveLoadManager();
    ConfigManager& getConfigManager();
    LogManager& getLogManager();
    TaxManager& getTaxManager();
    FestivalManager& getFestivalManager();
    WinConditionManager& getWinConditionManager();
    Dice& getDice();
    vector<Player>& getPlayers();
    bool isGameOver() const;
};