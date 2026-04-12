#pragma once

#include "model/cards/Card.hpp"

using namespace std;

class Game;
class Player;

class InstantCard : public Card {
public:
    InstantCard();
    InstantCard(const string& name, const string& description);
    InstantCard(const InstantCard& other);
    virtual ~InstantCard();
    InstantCard& operator=(const InstantCard& other);

    virtual void apply(Game& game, Player& player) override = 0;
};