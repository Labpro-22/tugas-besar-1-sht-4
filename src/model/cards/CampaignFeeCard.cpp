#include "model/cards/CampaignFeeCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

using namespace std;

CampaignFeeCard::CampaignFeeCard()
    : CommunityChestCard(), amountPerPlayer(0) {}

CampaignFeeCard::CampaignFeeCard(const string& name, const string& description, int amountPerPlayer)
    : CommunityChestCard(name, description), amountPerPlayer(amountPerPlayer) {}

CampaignFeeCard::CampaignFeeCard(const CampaignFeeCard& other)
    : CommunityChestCard(other), amountPerPlayer(other.amountPerPlayer) {}

CampaignFeeCard::~CampaignFeeCard() {}

CampaignFeeCard& CampaignFeeCard::operator=(const CampaignFeeCard& other) {
    if (this != &other) {
        CommunityChestCard::operator=(other);
        amountPerPlayer = other.amountPerPlayer;
    }
    return *this;
}

void CampaignFeeCard::apply(Game& game, Player& player) {
    int activeOthers = 0;
    for (Player& other : game.getPlayers()) {
        if (other.getUsername() == player.getUsername()) continue;
        if (other.isBankrupt()) continue;
        activeOthers++;
    }
    int totalOwed = amountPerPlayer * activeOthers;

    if (player.getMoney() < totalOwed) {
        game.getLogManager().addLog(
            game.getCurrentTurn(),
            player.getUsername(),
            "BANGKRUT",
            "Tidak mampu membayar biaya kampanye M" + to_string(totalOwed)
        );

        game.getBankruptcyManager().beginBankruptcySession(player, nullptr, totalOwed, true);
        return;
    }

    for (Player& other : game.getPlayers()) {
        if (other.getUsername() == player.getUsername()) continue;
        if (other.isBankrupt()) continue;

        player -= amountPerPlayer;
        other += amountPerPlayer;

        game.getLogManager().addLog(
            game.getCurrentTurn(),
            player.getUsername(),
            "KARTU",
            "Membayar biaya kampanye M" + to_string(amountPerPlayer) + " ke " + other.getUsername()
        );
    }
}
