#include "model/cards/BirthdayCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>

using namespace std;

BirthdayCard::BirthdayCard()
    : CommunityChestCard(), amount(0) {}

BirthdayCard::BirthdayCard(const string& name, const string& description, int amount)
    : CommunityChestCard(name, description), amount(amount) {}

BirthdayCard::BirthdayCard(const BirthdayCard& other)
    : CommunityChestCard(other), amount(other.amount) {}

BirthdayCard::~BirthdayCard() {}

BirthdayCard& BirthdayCard::operator=(const BirthdayCard& other) {
    if (this != &other) {
        CommunityChestCard::operator=(other);
        amount = other.amount;
    }
    return *this;
}

void BirthdayCard::apply(Game& game, Player& player) {
    cout << "Kartu: \"Ini adalah hari ulang tahun Anda. Dapatkan M"
         << amount << " dari setiap pemain.\"" << endl;

    int totalReceived = 0;
    for (Player& other : game.getPlayers()) {
        if (other.getUsername() == player.getUsername()) continue;
        if (other.isBankrupt()) continue;

        int pay = min(amount, other.getMoney());
        other -= pay;
        player += pay;
        totalReceived += pay;

        cout << other.getUsername() << " membayar M" << pay
             << " kepada " << player.getUsername() << "." << endl;

        game.getLogManager().addLog(
            game.getCurrentTurn(),
            other.getUsername(),
            "KARTU",
            "Membayar ulang tahun M" + to_string(pay) + " ke " + player.getUsername()
        );
    }

    cout << player.getUsername() << " mendapatkan total M"
         << totalReceived << " dari hadiah ulang tahun." << endl;

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menerima hadiah ulang tahun total M" + to_string(totalReceived)
    );
}
