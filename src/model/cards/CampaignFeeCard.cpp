#include "model/cards/CampaignFeeCard.hpp"

using namespace std;

CampaignFeeCard::CampaignFeeCard() {}

CampaignFeeCard::CampaignFeeCard(const string& name, const string& description, int amountPerPlayer) {}

CampaignFeeCard::CampaignFeeCard(const CampaignFeeCard& other) {}

CampaignFeeCard::~CampaignFeeCard() {}

CampaignFeeCard& CampaignFeeCard::operator=(const CampaignFeeCard& other) {
    return *this;
}

void CampaignFeeCard::apply(Game& game, Player& player) {}