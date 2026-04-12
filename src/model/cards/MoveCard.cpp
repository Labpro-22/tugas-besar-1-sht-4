#include "model/cards/MoveCard.hpp"

using namespace std;

MoveCard::MoveCard() {}

MoveCard::MoveCard(const string& name, const string& description, bool used, int steps) {}

MoveCard::MoveCard(const MoveCard& other) {}

MoveCard::~MoveCard() {}

MoveCard& MoveCard::operator=(const MoveCard& other) {
    return *this;
}

void MoveCard::apply(Game& game, Player& player) {}