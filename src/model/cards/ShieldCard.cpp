#include "model/cards/ShieldCard.hpp"

using namespace std;

ShieldCard::ShieldCard() : HandCard(), duration(0) {}

ShieldCard::ShieldCard(const string& name, const string& description, bool used, int duration)
    : HandCard(name, description, used), duration(duration) {}

ShieldCard::ShieldCard(const ShieldCard& other) : HandCard(other), duration(other.duration) {}

ShieldCard::~ShieldCard() {}

ShieldCard& ShieldCard::operator=(const ShieldCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        duration = other.duration;
    }
    return *this;
}

void ShieldCard::apply(Game& game, Player& player) {}
