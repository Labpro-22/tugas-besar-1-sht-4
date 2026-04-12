#pragma once

#include <vector>

using namespace std;

class Game;
class Player;

class TurnManager {
private:
    int currentPlayerIndex;
    vector<int> turnOrder;
    bool rolledThisTurn;
    int consecutiveDoubles;
    int turnCount;

public:
    TurnManager();
    TurnManager(int currentPlayerIndex, const vector<int>& turnOrder, bool rolledThisTurn, int consecutiveDoubles, int turnCount);
    TurnManager(const TurnManager& other);
    ~TurnManager();
    TurnManager& operator=(const TurnManager& other);

    void initializeTurnOrder(int playerCount);
    Player& getCurrentPlayer(Game& game);
    void nextPlayer(Game& game);
    void startTurn(Game& game);
    void endTurn(Game& game);
    void registerDiceResult(bool isDouble);
    bool canRollDice() const;
    void updateTurnEffects(Game& game, Player& player);
};