#pragma once

#include "model/cards/InstantCard.hpp"

using namespace std;

class Game;
class Player;

class CommunityChestCard : public InstantCard {
public:
    CommunityChestCard();
    CommunityChestCard(const string& name, const string& description);
    CommunityChestCard(const CommunityChestCard& other);
    virtual ~CommunityChestCard();
    CommunityChestCard& operator=(const CommunityChestCard& other);

    virtual void apply(Game& game, Player& player) override = 0;
};