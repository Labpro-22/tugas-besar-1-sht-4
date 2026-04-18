#include "model/cards/HandCard.hpp"

using namespace std;

HandCard::HandCard()
    : Card(), used(false) {}

HandCard::HandCard(const string& name, const string& description, bool used)
    : Card(name, description), used(used) {}

HandCard::HandCard(const HandCard& other)
    : Card(other), used(other.used) {}

HandCard::~HandCard() {}

HandCard& HandCard::operator=(const HandCard& other) {
    if (this != &other) {
        Card::operator=(other);
        used = other.used;
    }
    return *this;
}