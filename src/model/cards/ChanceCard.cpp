#include "model/cards/ChanceCard.hpp"

using namespace std;

ChanceCard::ChanceCard()
    : InstantCard() {}

ChanceCard::ChanceCard(const string& name, const string& description)
    : InstantCard(name, description) {}

ChanceCard::ChanceCard(const ChanceCard& other)
    : InstantCard(other) {}

ChanceCard::~ChanceCard() {}

ChanceCard& ChanceCard::operator=(const ChanceCard& other) {
    if (this != &other) {
        InstantCard::operator=(other);
    }
    return *this;
}