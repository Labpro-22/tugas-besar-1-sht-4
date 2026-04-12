#include "model/cards/MoveBackThreeCard.hpp"

using namespace std;

MoveBackThreeCard::MoveBackThreeCard() {}

MoveBackThreeCard::MoveBackThreeCard(const string& name, const string& description) {}

MoveBackThreeCard::MoveBackThreeCard(const MoveBackThreeCard& other) {}

MoveBackThreeCard::~MoveBackThreeCard() {}

MoveBackThreeCard& MoveBackThreeCard::operator=(const MoveBackThreeCard& other) {
    return *this;
}

void MoveBackThreeCard::apply(Game& game, Player& player) {}