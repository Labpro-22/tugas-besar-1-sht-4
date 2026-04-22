#pragma once

#include "model/cards/HandCard.hpp"

using namespace std;

class Game;
class Player;

class LassoCard : public HandCard {
public:
    LassoCard();
    LassoCard(const string& name, const string& description, bool used);
    LassoCard(const LassoCard& other);
    ~LassoCard();
    LassoCard& operator=(const LassoCard& other);

    void setTargetPlayer(Player* player);
    void apply(Game& game, Player& player) override;
private:
    Player* targetPlayer;
};