#include "model/cards/GoToNearestRailroadCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>

using namespace std;

GoToNearestRailroadCard::GoToNearestRailroadCard()
    : ChanceCard() {}

GoToNearestRailroadCard::GoToNearestRailroadCard(const string& name, const string& description)
    : ChanceCard(name, description) {}

GoToNearestRailroadCard::GoToNearestRailroadCard(const GoToNearestRailroadCard& other)
    : ChanceCard(other) {}

GoToNearestRailroadCard::~GoToNearestRailroadCard() {}

GoToNearestRailroadCard& GoToNearestRailroadCard::operator=(const GoToNearestRailroadCard& other) {
    if (this != &other) {
        ChanceCard::operator=(other);
    }
    return *this;
}

void GoToNearestRailroadCard::apply(Game& game, Player& player) {
    cout << "Kartu: \"Pergi ke stasiun terdekat.\"" << endl;
    game.getMovementManager().movePlayerToNearestRailroad(player);
}
