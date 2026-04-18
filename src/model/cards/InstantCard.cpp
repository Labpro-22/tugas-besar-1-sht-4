#include "model/cards/InstantCard.hpp"

using namespace std;

InstantCard::InstantCard()
    : Card() {}

InstantCard::InstantCard(const string& name, const string& description)
    : Card(name, description) {}

InstantCard::InstantCard(const InstantCard& other)
    : Card(other) {}

InstantCard::~InstantCard() {}

InstantCard& InstantCard::operator=(const InstantCard& other) {
    if (this != &other) {
        Card::operator=(other);
    }
    return *this;
}