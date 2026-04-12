#pragma once

#include "model/cards/ChanceCard.hpp"

using namespace std;

class Game;
class Player;

class GoToNearestRailroadCard : public ChanceCard {
public:
    GoToNearestRailroadCard();
    GoToNearestRailroadCard(const string& name, const string& description);
    GoToNearestRailroadCard(const GoToNearestRailroadCard& other);
    ~GoToNearestRailroadCard();
    GoToNearestRailroadCard& operator=(const GoToNearestRailroadCard& other);

    void apply(Game& game, Player& player) override;
};