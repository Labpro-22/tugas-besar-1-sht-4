#include "model/cards/ShieldCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>

using namespace std;

ShieldCard::ShieldCard()
    : HandCard(), duration(0) {}

ShieldCard::ShieldCard(const string& name, const string& description, bool used, int duration)
    : HandCard(name, description, used), duration(duration) {}

ShieldCard::ShieldCard(const ShieldCard& other)
    : HandCard(other), duration(other.duration) {}

ShieldCard::~ShieldCard() {}

ShieldCard& ShieldCard::operator=(const ShieldCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        duration = other.duration;
    }
    return *this;
}

void ShieldCard::apply(Game& game, Player& player) {
    cout << "ShieldCard diaktifkan! Anda kebal terhadap tagihan atau sanksi "
         << "selama giliran ini." << endl;

    player.setShieldActive(true);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan ShieldCard, kebal tagihan/sanksi selama 1 giliran"
    );
}
