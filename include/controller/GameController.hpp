#pragma once

#include <string>

using namespace std;

class Game;
class UIManager;
class CommandController;
class TileController;

class GameController {
private:
    Game& game;
    UIManager& uiManager;
    CommandController* commandController;
    TileController* tileController;
    bool rolledThisTurn;

    string toUpperCopy(string value) const;
    string firstToken(const string& input) const;
    bool isTurnEndingCommand(const string& input) const;
    size_t activePlayerCount(const Game& game) const;

public:
    GameController(Game& game, UIManager& uiManager);
    GameController(const GameController& other);
    ~GameController();
    GameController& operator=(const GameController& other);

    void runGameLoop();
    void runTurn();

    void handleStartTurn();
    void handleEndTurn();

    bool canSaveNow() const;
    bool isCommandValidThisTurn(const string& input) const;
};
