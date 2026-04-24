#include "model/cards/BirthdayCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

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
    int totalReceived = 0;
    for (Player& other : game.getPlayers()) {
        if (other.getUsername() == player.getUsername()) continue;
        if (other.isBankrupt()) continue;

        if (other.getMoney() >= amount) {
            other -= amount;
            player += amount;
            totalReceived += amount;

            game.getLogManager().addLog(
                game.getCurrentTurn(),
                other.getUsername(),
                "KARTU",
                "Membayar ulang tahun M" + to_string(amount) + " ke " + player.getUsername()
            );
        } else {
            game.getLogManager().addLog(
                game.getCurrentTurn(),
                other.getUsername(),
                "BANGKRUT",
                "Tidak mampu membayar ulang tahun M" + to_string(amount) + " ke " + player.getUsername()
            );
            game.getBankruptcyManager().beginBankruptcySession(other, &player, amount, false);
        }
    }

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menerima hadiah ulang tahun total M" + to_string(totalReceived)
    );
}
