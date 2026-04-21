#include "model/cards/DoctorFeeCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>

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
    cout << "Kartu: \"Biaya dokter. Bayar M" << fee << ".\"" << endl;

    if (player.getMoney() >= fee) {
        player -= fee;
        cout << "Kamu membayar M" << fee << " ke Bank. "
             << "Sisa Uang = M" << player.getMoney() << "." << endl;

        game.getLogManager().addLog(
            game.getCurrentTurn(),
            player.getUsername(),
            "KARTU",
            "Membayar biaya dokter M" + to_string(fee) + " ke Bank"
        );
    } else {
        cout << "Kamu tidak mampu membayar biaya dokter! (M" << fee << ")" << endl;
        cout << "Uang kamu saat ini: M" << player.getMoney() << endl;

        game.getLogManager().addLog(
            game.getCurrentTurn(),
            player.getUsername(),
            "BANGKRUT",
            "Tidak mampu membayar biaya dokter M" + to_string(fee)
        );

        game.getBankruptcyManager().beginBankruptcySession(player, nullptr, fee, true);
    }
}
