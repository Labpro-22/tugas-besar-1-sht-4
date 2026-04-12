#include "model/cards/TeleportCard.hpp"

using namespace std;

TeleportCard::TeleportCard() {}

TeleportCard::TeleportCard(const string& name, const string& description, bool used) {}

TeleportCard::TeleportCard(const TeleportCard& other) {}

TeleportCard::~TeleportCard() {}

TeleportCard& TeleportCard::operator=(const TeleportCard& other) {
    return *this;
}

void TeleportCard::apply(Game& game, Player& player) {}