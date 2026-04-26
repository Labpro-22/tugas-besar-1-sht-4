#include "model/cards/DoctorFeeCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

using namespace std;

DoctorFeeCard::DoctorFeeCard()
    : CommunityChestCard(), fee(0) {}

DoctorFeeCard::DoctorFeeCard(const string& name, const string& description, int fee)
    : CommunityChestCard(name, description), fee(fee) {}

DoctorFeeCard::DoctorFeeCard(const DoctorFeeCard& other)
    : CommunityChestCard(other), fee(other.fee) {}

DoctorFeeCard::~DoctorFeeCard() {}

DoctorFeeCard& DoctorFeeCard::operator=(const DoctorFeeCard& other) {
    if (this != &other) {
        CommunityChestCard::operator=(other);
        fee = other.fee;
    }
    return *this;
}

void DoctorFeeCard::apply(Game& game, Player& player) {
    const int effectiveFee = player.effectiveCost(fee);
    if (player.getMoney() >= effectiveFee) {
        player -= fee;

        game.getLogManager().addLog(
            game.getCurrentTurn(),
            player.getUsername(),
            "KARTU",
            "Membayar biaya dokter M" + to_string(effectiveFee) + " ke Bank"
        );
    } else {
        game.getLogManager().addLog(
            game.getCurrentTurn(),
            player.getUsername(),
            "BANGKRUT",
            "Tidak mampu membayar biaya dokter M" + to_string(effectiveFee)
        );

        game.getBankruptcyManager().beginBankruptcySession(player, nullptr, fee, true);
    }
}
