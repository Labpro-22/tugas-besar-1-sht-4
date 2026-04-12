#include "model/cards/HandCard.hpp"

using namespace std;

HandCard::HandCard() {}

HandCard::HandCard(const string& name, const string& description, bool used) {}

HandCard::HandCard(const HandCard& other) {}

HandCard::~HandCard() {}

HandCard& HandCard::operator=(const HandCard& other) {
    return *this;
}