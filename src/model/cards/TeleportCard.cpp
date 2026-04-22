#include "model/cards/TeleportCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/Board.hpp"

#include <string>

using namespace std;

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

    string targetCode = tile->getCode();

    game.getMovementManager().movePlayerTo(player, targetTileIndex);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan TeleportCard, pindah ke " + targetCode
    );
}