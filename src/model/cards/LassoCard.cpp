#include "model/cards/LassoCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>
#include <vector>

using namespace std;

LassoCard::LassoCard()
    : HandCard() {}

LassoCard::LassoCard(const string& name, const string& description, bool used)
    : HandCard(name, description, used) {}

LassoCard::LassoCard(const LassoCard& other)
    : HandCard(other) {}

LassoCard::~LassoCard() {}

LassoCard& LassoCard::operator=(const LassoCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
    }
    return *this;
}

void LassoCard::apply(Game& game, Player& player) {
    cout << "LassoCard diaktifkan! Pilih lawan yang ingin ditarik." << endl;

    int myPos = player.getPosition();

    vector<Player*> targets;
    int boardSize = game.getBoard().getBoardSize();

    for (Player& other : game.getPlayers()) {
        if (other.getUsername() == player.getUsername()) continue;
        if (other.isBankrupt()) continue;

        int otherPos = other.getPosition();
        bool isAhead = (otherPos > myPos) ||
                       (otherPos < myPos && (myPos - otherPos) > boardSize / 2);
        if (isAhead) {
            targets.push_back(&other);
        }
    }

    if (targets.empty()) {
        cout << "Tidak ada lawan yang berada di depan posisi Anda. "
             << "LassoCard tidak berpengaruh." << endl;
        return;
    }

    cout << "\nPilih lawan yang ingin ditarik ke posisi Anda:" << endl;
    for (int i = 0; i < (int)targets.size(); i++) {
        cout << i + 1 << ". " << targets[i]->getUsername()
             << " (petak ke-" << targets[i]->getPosition() << ")" << endl;
    }
    cout << "0. Batal" << endl;

    int choice = -1;
    while (choice < 0 || choice > (int)targets.size()) {
        cout << "Pilihan (0-" << targets.size() << "): ";
        cin >> choice;
    }

    if (choice == 0) {
        cout << "LassoCard dibatalkan." << endl;
        return;
    }

    Player* target = targets[choice - 1];
    cout << target->getUsername() << " ditarik ke posisi "
         << player.getUsername() << " (petak ke-" << myPos << ")." << endl;

    target->moveTo(myPos);

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan LassoCard, menarik " + target->getUsername() +
        " ke petak " + to_string(myPos)
    );
}