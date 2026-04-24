#include "model/cards/MoveCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

using namespace std;

MoveCard::MoveCard()
    : HandCard(), steps(0) {}

MoveCard::MoveCard(const string& name, const string& description, bool used, int steps)
    : HandCard(name, description, used), steps(steps) {}

MoveCard::MoveCard(const MoveCard& other)
    : HandCard(other), steps(other.steps) {}

MoveCard::~MoveCard() {}

MoveCard& MoveCard::operator=(const MoveCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        steps = other.steps;
    }
    return *this;
}

int MoveCard::getSteps() const { return steps; }

void MoveCard::apply(Game& game, Player& player) {
    const int boardSize = game.getBoard().getBoardSize();
    int newPos = player.getPosition() + steps;
    if (boardSize > 0) {
        newPos = ((newPos - 1) % boardSize + boardSize) % boardSize + 1;
    }
    game.getMovementManager().movePlayerTo(player, newPos);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan MoveCard, maju " + to_string(steps) + " petak"
    );
}
