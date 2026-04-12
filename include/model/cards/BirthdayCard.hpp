#pragma once

#include "model/cards/CommunityChestCard.hpp"

using namespace std;

class Game;
class Player;

class BirthdayCard : public CommunityChestCard {
private:
    int amount;

public:
    BirthdayCard();
    BirthdayCard(const string& name, const string& description, int amount);
    BirthdayCard(const BirthdayCard& other);
    ~BirthdayCard();
    BirthdayCard& operator=(const BirthdayCard& other);

    void apply(Game& game, Player& player) override;
};