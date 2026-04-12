#pragma once

#include "model/cards/CommunityChestCard.hpp"

using namespace std;

class Game;
class Player;

class CampaignFeeCard : public CommunityChestCard {
private:
    int amountPerPlayer;

public:
    CampaignFeeCard();
    CampaignFeeCard(const string& name, const string& description, int amountPerPlayer);
    CampaignFeeCard(const CampaignFeeCard& other);
    ~CampaignFeeCard();
    CampaignFeeCard& operator=(const CampaignFeeCard& other);

    void apply(Game& game, Player& player) override;
};