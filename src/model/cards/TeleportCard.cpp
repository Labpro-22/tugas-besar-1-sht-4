#include "model/cards/TeleportCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/Board.hpp"
#include "model/tiles/Tile.hpp"

#include <string>

using namespace std;

namespace {
string tileCode(const Game& game, int position) {
    auto tile = game.getBoard().getTile(position);
    return tile != nullptr ? tile->getCode() : to_string(position);
}
}

TeleportCard::TeleportCard()
    : HandCard(), targetTileIndex(-1) {}

TeleportCard::TeleportCard(const string& name, const string& description, bool used)
    : HandCard(name, description, used), targetTileIndex(-1) {}

TeleportCard::TeleportCard(const TeleportCard& other)
    : HandCard(other), targetTileIndex(other.targetTileIndex) {}

TeleportCard::~TeleportCard() {}

TeleportCard& TeleportCard::operator=(const TeleportCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        targetTileIndex = other.targetTileIndex;
    }
    return *this;
}

void TeleportCard::setTargetTileIndex(int index) {
    targetTileIndex = index;
}

void TeleportCard::apply(Game& game, Player& player) {
    if (targetTileIndex < 0) return;

    Board& board = game.getBoard();
    auto tile = board.getTile(targetTileIndex);
    if (!tile) return;

    const int previousPosition = player.getPosition();
    string targetCode = tile->getCode();

    game.getMovementManager().movePlayerTo(player, targetTileIndex);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "GERAK",
        "TeleportCard memindahkan bidak dari " + tileCode(game, previousPosition) + " ke " + tileCode(game, player.getPosition())
    );

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan TeleportCard, pindah ke " + targetCode
    );
}
