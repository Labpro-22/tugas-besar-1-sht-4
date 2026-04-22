#include "model/cards/LassoCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <vector>

using namespace std;

LassoCard::LassoCard()
    : HandCard(), targetPlayer(nullptr) {}

LassoCard::LassoCard(const string& name, const string& description, bool used)
    : HandCard(name, description, used), targetPlayer(nullptr) {}

LassoCard::LassoCard(const LassoCard& other)
    : HandCard(other), targetPlayer(other.targetPlayer) {}

LassoCard::~LassoCard() {}

LassoCard& LassoCard::operator=(const LassoCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        targetPlayer = other.targetPlayer;
    }
    return *this;
}

void LassoCard::setTargetPlayer(Player* player) {
    targetPlayer = player;
}

void LassoCard::apply(Game& game, Player& player) {
    if (!targetPlayer) return;

    int myPos = player.getPosition();
    targetPlayer->moveTo(myPos);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan LassoCard, menarik " + targetPlayer->getUsername() +
        " ke petak " + to_string(myPos)
    );
}