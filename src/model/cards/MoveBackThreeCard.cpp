#include "model/cards/MoveBackThreeCard.hpp"
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
    const int boardSize = game.getBoard().getBoardSize();
    const int previousPosition = player.getPosition();
    int newPos = player.getPosition() - 3;
    if (newPos < 1) newPos += boardSize;
    player.moveTo(newPos);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "GERAK",
        "MoveBackThree memindahkan bidak mundur 3 petak dari " + tileCode(game, previousPosition) + " ke " + tileCode(game, player.getPosition())
    );

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan MoveBackThree."
    );
}
