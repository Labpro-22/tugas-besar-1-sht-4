#include "model/cards/DemolitionCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"
#include "model/tiles/StreetTile.hpp"

#include <vector>

using namespace std;

DemolitionCard::DemolitionCard()
    : HandCard(), targetStreet(nullptr) {}

DemolitionCard::DemolitionCard(const string& name, const string& description, bool used)
    : HandCard(name, description, used), targetStreet(nullptr) {}

DemolitionCard::DemolitionCard(const DemolitionCard& other)
    : HandCard(other), targetStreet(other.targetStreet) {}

DemolitionCard::~DemolitionCard() {}

DemolitionCard& DemolitionCard::operator=(const DemolitionCard& other) {
    if (this != &other) {
        HandCard::operator=(other);
        targetStreet = other.targetStreet;
    }
    return *this;
}

void DemolitionCard::setTargetStreet(StreetTile* street) {
    targetStreet = street;
}

void DemolitionCard::apply(Game& game, Player& player) {
    if (!targetStreet) return;
    
    Player* owner = targetStreet->getOwner();
    string ownerName = owner ? owner->getUsername() : "BANK";

    targetStreet->sellBuildings();

    game.getLogManager().addLog(
        game.getCurrentTurn(),
        player.getUsername(),
        "KARTU",
        "Menggunakan DemolitionCard, menghancurkan bangunan di " +
        targetStreet->getCode() + " milik " + ownerName
    );
}