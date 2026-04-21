#include "model/tiles/RailroadTile.hpp"
#include "model/Game.hpp"
#include "model/NimonException.hpp"
#include "model/Player.hpp"

using namespace std;

RailroadTile::RailroadTile() {}

RailroadTile::RailroadTile(
    int index,
    const string& code,
    const string& name,
    Player* owner,
    OwnershipStatus ownershipStatus,
    int purchasePrice,
    int mortgageValue
)
    : OwnableTile(index, code, name, owner, ownershipStatus, purchasePrice, mortgageValue)
{}

RailroadTile::RailroadTile(const RailroadTile& other)
    : OwnableTile(other)
{}

RailroadTile::~RailroadTile() {}

RailroadTile& RailroadTile::operator=(const RailroadTile& other) {
    if (this != &other) {
        OwnableTile::operator=(other);
    }
    return *this;
}

int RailroadTile::calculateRent(const Game& game, const Player& visitor) const {
    if (this->getOwner() == nullptr) return 0;
    if (this->getOwnershipStatus() == OwnershipStatus::MORTGAGED) return 0;
    int count = game.getBoard().countRailroadsOwned(*this->getOwner());
    return game.getConfigManager().getRailroadRent(count);  
}

void RailroadTile::onLand(Game& game, Player& player) {
    // TODO : implement ui or something
}

void RailroadTile::acquire(Game& game, Player& player) {
    if (this->getOwner() != nullptr) throw InvalidActionException("Railroad already has an owner");
    if (this->getOwnershipStatus() != OwnershipStatus::BANK) throw InvalidActionException("Railroad is not in BANK status");
    this->setOwner(&player);
    this->ownershipStatus == OwnershipStatus::OWNED;
    player.addProperty(this);

    // TODO : add log if needed
}

int RailroadTile::getBuildingValue() const {
    return 0;
}

int RailroadTile::getSelltoBankValue() const {
    return purchasePrice;
}