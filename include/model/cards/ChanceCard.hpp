#pragma once

#include "model/cards/InstantCard.hpp"

using namespace std;

class Game;
class Player;

class ChanceCard : public InstantCard {
public:
    ChanceCard();
    ChanceCard(const string& name, const string& description);
    ChanceCard(const ChanceCard& other);
    virtual ~ChanceCard();
    ChanceCard& operator=(const ChanceCard& other);

    virtual void apply(Game& game, Player& player) override = 0;
};