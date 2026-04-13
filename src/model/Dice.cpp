#include "model/Dice.hpp"

using namespace std;

Dice::Dice(): die1(1), die2(1), manualMode(false) {}

Dice::Dice(int die1, int die2, bool manualMode):manualMode(manualMode) {
    if (die1<1 || die1>6 || die2<1 || die2>6) {
        throw InvalidActionException("Angka harus berada antara 1-6");
    }
    this->die1=die1;
    this->die2=die2;
}

Dice::Dice(const Dice& other) {
    die1=other.die1;
    die2=other.die2;
    manualMode=other.manualMode;
}

Dice::~Dice() {}

Dice& Dice::operator=(const Dice& other) {
    if (this!=&other){
        die1=other.die1;
        die2=other.die2;
        manualMode=other.manualMode;
    }
    return *this;
}

void Dice::roll() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(1, 6);

    die1=dist(rng);
    die2=dist(rng);
}

void Dice::setManual(int x, int y) {
    if (manualMode){
        if (x>=1 && x<=6 && y>=1 && y<=6){
            die1=x;
            die2=y;
            return;
        }
        throw InvalidActionException("Angka harus berada antara 1-6");
    }
    throw InvalidActionException("Tidak berada pada mode manual");
}

int Dice::getDie1() const {
    return die1;
}

int Dice::getDie2() const {
    return die2;
}

int Dice::getTotal() const {
    return die1+die2;
}

bool Dice::isDouble() const {
    return die1==die2;
}