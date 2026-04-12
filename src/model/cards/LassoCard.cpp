#include "model/cards/LassoCard.hpp"

using namespace std;

LassoCard::LassoCard() {}

LassoCard::LassoCard(const string& name, const string& description, bool used) {}

LassoCard::LassoCard(const LassoCard& other) {}

LassoCard::~LassoCard() {}

LassoCard& LassoCard::operator=(const LassoCard& other) {
    return *this;
}

void LassoCard::apply(Game& game, Player& player) {}