#pragma once

#include <vector>

using namespace std;

#include "model/GameContext.hpp"

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
    Player& getCurrentPlayer(GameContext& gameContext);
    void nextPlayer(GameContext& gameContext);
    void startTurn(GameContext& gameContext);
    void endTurn(GameContext& gameContext);
    void registerDiceResult(bool isDouble);
    bool canRollDice() const;
    void updateTurnEffects(GameContext& gameContext, Player& player);
};