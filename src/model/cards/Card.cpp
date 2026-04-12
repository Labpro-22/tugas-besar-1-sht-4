#include "model/cards/Card.hpp"

using namespace std;

Card::Card() {}

Card::Card(const string& name, const string& description) {}

Card::Card(const Card& other) {}

Card::~Card() {}

Card& Card::operator=(const Card& other) {
    return *this;
}

string Card::getName() const {
    return name;
}