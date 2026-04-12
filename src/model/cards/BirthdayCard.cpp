#include "model/cards/BirthdayCard.hpp"

using namespace std;

BirthdayCard::BirthdayCard() {}

BirthdayCard::BirthdayCard(const string& name, const string& description, int amount) {}

BirthdayCard::BirthdayCard(const BirthdayCard& other) {}

BirthdayCard::~BirthdayCard() {}

BirthdayCard& BirthdayCard::operator=(const BirthdayCard& other) {
    return *this;
}

void BirthdayCard::apply(Game& game, Player& player) {}