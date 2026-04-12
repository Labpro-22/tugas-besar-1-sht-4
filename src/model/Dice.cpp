#include "model/Dice.hpp"

using namespace std;

Dice::Dice() {}

Dice::Dice(int die1, int die2, bool manualMode) {}

Dice::Dice(const Dice& other) {}

Dice::~Dice() {}

Dice& Dice::operator=(const Dice& other) {
    return *this;
}

void Dice::roll() {}

void Dice::setManual(int x, int y) {}

int Dice::getDie1() const {
    return die1;
}

int Dice::getDie2() const {
    return die2;
}

int Dice::getTotal() const {
    return 0;
}

bool Dice::isDouble() const {
    return false;
}