#include "model/tiles/UtilityTile.hpp"
#include "model/RentContext.hpp"
#include "model/NimonException.hpp"
#include "model/Player.hpp"

using namespace std;

UtilityTile::UtilityTile() {}

UtilityTile::UtilityTile(
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

UtilityTile::UtilityTile(const UtilityTile& other)
    : OwnableTile(other)
{}

UtilityTile::~UtilityTile() {}

UtilityTile& UtilityTile::operator=(const UtilityTile& other) {
    if (this != &other) {
        OwnableTile::operator=(other);
    }
    return *this;
}

int UtilityTile::calculateRent(const RentContext& rentContext) const {
    if (this->getOwner() == nullptr) return 0;
    if (this->getOwnershipStatus() == OwnershipStatus::MORTGAGED) return 0;

    return rentContext.getDiceTotal() * rentContext.getUtilityMultiplier();
}

Tile::TileType UtilityTile::onLand() const {
    return TileType::Utility;
}

void UtilityTile::acquire(Player& player) {
    if (this->getOwner() != nullptr) throw InvalidActionException("Utility already has an owner");
    if (this->getOwnershipStatus() != OwnershipStatus::BANK) throw InvalidActionException("Utility is not in BANK status");
    this->setOwner(&player);
    this->ownershipStatus = OwnershipStatus::OWNED;
}

int UtilityTile::getBuildingValue() const {
    return 0;
}

int UtilityTile::getSelltoBankValue() const {
    return purchasePrice;
}
