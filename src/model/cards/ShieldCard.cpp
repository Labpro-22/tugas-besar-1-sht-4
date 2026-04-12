#include "model/cards/ShieldCard.hpp"

using namespace std;

ShieldCard::ShieldCard() {}

ShieldCard::ShieldCard(const string& name, const string& description, bool used, int duration) {}

ShieldCard::ShieldCard(const ShieldCard& other) {}

ShieldCard::~ShieldCard() {}

ShieldCard& ShieldCard::operator=(const ShieldCard& other) {
    return *this;
}

void ShieldCard::apply(Game& game, Player& player) {}