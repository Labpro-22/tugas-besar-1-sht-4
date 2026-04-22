#include "model/cards/DiscountCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>

using namespace std;

DiscountCard::DiscountCard()
    : HandCard(), discountPercent(0), duration(0) {}

DiscountCard::DiscountCard(const string& name, const string& description, bool used, int discountPercent, int duration)
    : HandCard(name, description, used), discountPercent(discountPercent), duration(duration) {}

DiscountCard::DiscountCard(const DiscountCard& other)
    : HandCard(other), discountPercent(other.discountPercent), duration(other.duration) {}

DiscountCard::~DiscountCard() {}

DiscountCard& DiscountCard::operator=(const DiscountCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        discountPercent = other.discountPercent;
        duration = other.duration;
    }
    return *this;
}

void DiscountCard::apply(Game& game, Player& player) {
    cout << "DiscountCard diaktifkan! Diskon " << discountPercent
         << "% berlaku selama giliran ini." << endl;

    player.setDiscount(discountPercent, duration);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan DiscountCard, diskon " + to_string(discountPercent) + "% selama " + to_string(duration) + " giliran"
    );
}