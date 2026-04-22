#pragma once

#include "model/cards/HandCard.hpp"

using namespace std;

class Game;
class Player;

class DiscountCard : public HandCard {
private:
    int discountPercent;
    int duration;

public:
    DiscountCard();
    DiscountCard(const string& name, const string& description, bool used, int discountPercent, int duration);
    DiscountCard(const DiscountCard& other);
    ~DiscountCard();
    DiscountCard& operator=(const DiscountCard& other);

    void apply(Game& game, Player& player) override;
    int getDiscountPercent() const;
    int getDuration() const;
};