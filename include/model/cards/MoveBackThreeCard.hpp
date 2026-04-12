#pragma once

#include "model/cards/ChanceCard.hpp"

using namespace std;

class Game;
class Player;

class MoveBackThreeCard : public ChanceCard {
public:
    MoveBackThreeCard();
    MoveBackThreeCard(const string& name, const string& description);
    MoveBackThreeCard(const MoveBackThreeCard& other);
    ~MoveBackThreeCard();
    MoveBackThreeCard& operator=(const MoveBackThreeCard& other);

    void apply(Game& game, Player& player) override;
};