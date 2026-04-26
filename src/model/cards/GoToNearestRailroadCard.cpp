#include "model/cards/GoToNearestRailroadCard.hpp"
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
    const int previousPosition = player.getPosition();
    game.getMovementManager().movePlayerToNearestRailroad(player, game.getBoard());
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "GERAK",
        "GoToNearestRailroad memindahkan bidak dari " + tileCode(game, previousPosition) + " ke " + tileCode(game, player.getPosition())
    );
    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan GoToNearestRailroad."
    );
}
