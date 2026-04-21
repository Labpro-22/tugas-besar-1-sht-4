#pragma once

#include <vector>

#include "model/Board.hpp"
#include "model/Player.hpp"

class GameContext {
private:
    int currentTurn;
    int maxTurn;
    bool isRunning;
    Board board;
    std::vector<Player> players;
public:
    GameContext();
    GameContext(int currentTurn, int maxTurn, bool isRunning);
    GameContext(const GameContext& other);
    ~GameContext();
    GameContext& operator=(const GameContext& other);

    int getCurrentTurn() const;
    int getMaxTurn() const;
    bool isGameRunning() const;
    Board& getBoard();
    const Board& getBoard() const;
    std::vector<Player>& getPlayers();
    const std::vector<Player>& getPlayers() const;

    void setCurrentTurn(int currentTurn);
    void setIsRunning(bool isRunning);
};
