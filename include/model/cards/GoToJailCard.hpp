#pragma once

#include "model/cards/ChanceCard.hpp"

using namespace std;

class Game;
class Player;

class GoToJailCard : public ChanceCard {
public:
    GoToJailCard();
    GoToJailCard(const string& name, const string& description);
    GoToJailCard(const GoToJailCard& other);
    ~GoToJailCard();
    GoToJailCard& operator=(const GoToJailCard& other);

    void apply(Game& game, Player& player) override;
};