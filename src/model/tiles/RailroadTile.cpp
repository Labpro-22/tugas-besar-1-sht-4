#include "model/tiles/RailroadTile.hpp"
#include "model/RentContext.hpp"
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

int RailroadTile::calculateRent(const RentContext& rentContext) const {
    if (this->getOwner() == nullptr) return 0;
    if (this->getOwnershipStatus() == OwnershipStatus::MORTGAGED) return 0;
    return rentContext.getRailroadRent();
}

Tile::TileType RailroadTile::onLand() const {
    return TileType::Railroad;
}

void RailroadTile::acquire(Player& player) {
    if (this->getOwner() != nullptr) throw InvalidActionException("Railroad already has an owner");
    if (this->getOwnershipStatus() != OwnershipStatus::BANK) throw InvalidActionException("Railroad is not in BANK status");
    this->setOwner(&player);
    this->ownershipStatus = OwnershipStatus::OWNED;
    player.addProperty(this);

    // TODO : add log if needed
}

int RailroadTile::getBuildingValue() const {
    return 0;
}

int RailroadTile::getSelltoBankValue() const {
    return purchasePrice;
}