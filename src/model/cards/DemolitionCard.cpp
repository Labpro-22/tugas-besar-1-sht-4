#include "model/cards/DemolitionCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/tiles/StreetTile.hpp"

#include <iostream>
#include <vector>

using namespace std;

DemolitionCard::DemolitionCard()
    : HandCard() {}

DemolitionCard::DemolitionCard(const string& name, const string& description, bool used)
    : HandCard(name, description, used) {}

DemolitionCard::DemolitionCard(const DemolitionCard& other)
    : HandCard(other) {}

DemolitionCard::~DemolitionCard() {}

DemolitionCard& DemolitionCard::operator=(const DemolitionCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
    }
    return *this;
}

void DemolitionCard::apply(Game& game, Player& player) {
    cout << "DemolitionCard diaktifkan! Pilih properti lawan yang ingin dihancurkan bangunannya." << endl;

    vector<StreetTile*> demolishableTargets;
    vector<string> ownerNames;

    for (Player& other : game.getPlayers()) {
        if (other.getUsername() == player.getUsername()) continue;
        if (other.isBankrupt()) continue;

        for (OwnableTile* tile : game.getPropertyManager().getOwnedProperties(game.getBoard(), other)) {
            StreetTile* street = dynamic_cast<StreetTile*>(tile);
            if (street && street->getBuildingLevel() > 0) {
                demolishableTargets.push_back(street);
                ownerNames.push_back(other.getUsername());
            }
        }
    }

    if (demolishableTargets.empty()) {
        cout << "Tidak ada properti lawan yang memiliki bangunan. "
             << "DemolitionCard tidak berpengaruh." << endl;
        return;
    }

    cout << "\nPilih properti yang bangunannya ingin dihancurkan:" << endl;
    for (int i = 0; i < (int)demolishableTargets.size(); i++) {
        StreetTile* st = demolishableTargets[i];
        string buildingDesc = st->hasHotel() ? "Hotel" :
                              to_string(st->getBuildingLevel()) + " Rumah";
        cout << i + 1 << ". [" << st->getCode() << "] " << st->getName()
             << " (milik " << ownerNames[i] << ", " << buildingDesc << ")" << endl;
    }
    cout << "0. Batal" << endl;

    int choice = -1;
    while (choice < 0 || choice > (int)demolishableTargets.size()) {
        cout << "Pilihan (0-" << demolishableTargets.size() << "): ";
        cin >> choice;
    }

    if (choice == 0) {
        cout << "DemolitionCard dibatalkan." << endl;
        return;
    }

    StreetTile* target = demolishableTargets[choice - 1];
    string ownerName = ownerNames[choice - 1];

    cout << "Bangunan di " << target->getName() << " (" << target->getCode()
         << ") milik " << ownerName << " dihancurkan!" << endl;

    target->sellBuildings();

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan DemolitionCard, menghancurkan bangunan di " +
        target->getCode() + " milik " + ownerName
    );
}