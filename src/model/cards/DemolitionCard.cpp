#include "model/cards/DemolitionCard.hpp"

using namespace std;

DemolitionCard::DemolitionCard() {}

DemolitionCard::DemolitionCard(const string& name, const string& description, bool used) {}

DemolitionCard::DemolitionCard(const DemolitionCard& other) {}

DemolitionCard::~DemolitionCard() {}

DemolitionCard& DemolitionCard::operator=(const DemolitionCard& other) {
    return *this;
}

void DemolitionCard::apply(Game& game, Player& player) {}