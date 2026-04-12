#pragma once

#include "model/cards/Card.hpp"

using namespace std;

class Game;
class Player;

class HandCard : public Card {
protected:
    bool used;

public:
    HandCard();
    HandCard(const string& name, const string& description, bool used);
    HandCard(const HandCard& other);
    virtual ~HandCard();
    HandCard& operator=(const HandCard& other);

    virtual void apply(Game& game, Player& player) override = 0;
};