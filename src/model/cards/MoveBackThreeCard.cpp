#include "model/cards/MoveBackThreeCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>

using namespace std;

MoveBackThreeCard::MoveBackThreeCard()
    : ChanceCard() {}

MoveBackThreeCard::MoveBackThreeCard(const string& name, const string& description)
    : ChanceCard(name, description) {}

MoveBackThreeCard::MoveBackThreeCard(const MoveBackThreeCard& other)
    : ChanceCard(other) {}

MoveBackThreeCard::~MoveBackThreeCard() {}

MoveBackThreeCard& MoveBackThreeCard::operator=(const MoveBackThreeCard& other) {
    if (this != &other) {
        ChanceCard::operator=(other);
    }
    return *this;
}

void MoveBackThreeCard::apply(Game& game, Player& player) {
    cout << "Kartu: \"Mundur 3 petak.\"" << endl;
    game.getMovementManager().movePlayerBack(game, player, 3);
}