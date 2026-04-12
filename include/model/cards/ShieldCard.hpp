#pragma once

#include "model/cards/HandCard.hpp"

using namespace std;

class Game;
class Player;

class ShieldCard : public HandCard {
private:
    int duration;

public:
    ShieldCard();
    ShieldCard(const string& name, const string& description, bool used, int duration);
    ShieldCard(const ShieldCard& other);
    ~ShieldCard();
    ShieldCard& operator=(const ShieldCard& other);

    void apply(Game& game, Player& player) override;
};