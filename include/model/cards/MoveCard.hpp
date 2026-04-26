#pragma once

#include "model/cards/HandCard.hpp"

using namespace std;

class Game;
class Player;

class MoveCard : public HandCard {
private:
    int steps;

    static string tileCode(const Game& game, int position);

public:
    MoveCard();
    MoveCard(const string& name, const string& description, bool used, int steps);
    MoveCard(const MoveCard& other);
    ~MoveCard();
    MoveCard& operator=(const MoveCard& other);

    void apply(Game& game, Player& player) override;
    int getSteps() const;
};
