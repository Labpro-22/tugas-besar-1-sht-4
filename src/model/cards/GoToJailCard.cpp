#include "model/cards/GoToJailCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

using namespace std;

GoToJailCard::GoToJailCard()
    : ChanceCard() {}

GoToJailCard::GoToJailCard(const string& name, const string& description)
    : ChanceCard(name, description) {}

GoToJailCard::GoToJailCard(const GoToJailCard& other)
    : ChanceCard(other) {}

GoToJailCard::~GoToJailCard() {}

GoToJailCard& GoToJailCard::operator=(const GoToJailCard& other) {
    if (this != &other) {
        ChanceCard::operator=(other);
    }
    return *this;
}

void GoToJailCard::apply(Game& game, Player& player) {
    game.getJailManager().sendToJail(player);
    const int jailIndex = game.getBoard().getTileIndex("PEN");
    player.moveTo(jailIndex >= 0 ? jailIndex : 11);
}
