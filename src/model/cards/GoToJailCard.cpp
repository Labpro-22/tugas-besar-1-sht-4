#include "model/cards/GoToJailCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/tiles/Tile.hpp"

using namespace std;

namespace {
string tileCode(const Game& game, int position) {
    auto tile = game.getBoard().getTile(position);
    return tile != nullptr ? tile->getCode() : to_string(position);
}
}

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
    const int previousPosition = player.getPosition();
    game.getJailManager().sendToJail(player);
    const int jailIndex = game.getBoard().getTileIndex("PEN");
    player.moveTo(jailIndex >= 0 ? jailIndex : 11);
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "GERAK",
        "GoToJail memindahkan bidak dari " + tileCode(game, previousPosition) + " ke " + tileCode(game, player.getPosition())
    );
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan GoToJail."
    );
}
