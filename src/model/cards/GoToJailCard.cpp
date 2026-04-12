#include "model/cards/GoToJailCard.hpp"

using namespace std;

GoToJailCard::GoToJailCard() {}

GoToJailCard::GoToJailCard(const string& name, const string& description) {}

GoToJailCard::GoToJailCard(const GoToJailCard& other) {}

GoToJailCard::~GoToJailCard() {}

GoToJailCard& GoToJailCard::operator=(const GoToJailCard& other) {
    return *this;
}

void GoToJailCard::apply(Game& game, Player& player) {}