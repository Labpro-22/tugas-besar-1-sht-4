#include "model/cards/TeleportCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/Board.hpp"

#include <iostream>
#include <string>

using namespace std;

TeleportCard::TeleportCard()
    : HandCard() {}

TeleportCard::TeleportCard(const string& name, const string& description, bool used)
    : HandCard(name, description, used) {}

TeleportCard::TeleportCard(const TeleportCard& other)
    : HandCard(other) {}

TeleportCard::~TeleportCard() {}

TeleportCard& TeleportCard::operator=(const TeleportCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
    }
    return *this;
}

void TeleportCard::apply(Game& game, Player& player) {
    cout << "TeleportCard diaktifkan! Masukkan kode petak tujuan." << endl;

    Board& board = game.getBoard();
    int boardSize = board.getBoardSize();

    cout << "\nDaftar petak yang tersedia:" << endl;
    for (int i = 0; i < boardSize; i++) {
        auto tile = board.getTile(i);
        if (tile) {
            cout << "  [" << tile->getCode() << "] " << tile->getName() << endl;
        }
    }

    string targetCode;
    int targetIndex = -1;
    while (targetIndex < 0) {
        cout << "\nMasukkan kode petak tujuan: ";
        cin >> targetCode;
        targetIndex = board.getTileIndex(targetCode);
        if (targetIndex < 0) {
            cout << "Kode petak tidak valid. Coba lagi." << endl;
        }
    }

    cout << "Bidak dipindahkan ke " << board.getTile(targetIndex)->getName()
         << " (" << targetCode << ")." << endl;

    game.getMovementManager().movePlayerTo(player, targetIndex);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan TeleportCard, pindah ke " + targetCode
    );
}