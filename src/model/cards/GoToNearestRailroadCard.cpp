#include "model/cards/GoToNearestRailroadCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

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
    game.getMovementManager().movePlayerToNearestRailroad(player);
}
