#include "model/tiles/UtilityTile.hpp"

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

int UtilityTile::calculateRent(const Game& game, const Player& visitor) const {
    if (this->getOwner() == nullptr) return 0;
    if (this->getOwnershipStatus() == OwnershipStatus::MORTGAGED) return 0;

    int count = game.getBoard().countUtilitiesOwned(*this->getOwner());
    // TODO : make sure get total is correct for getting dice
    return game.getDice().getTotal() * game.getConfigManager().getUtilityMultiplier(count);
}

void UtilityTile::onLand(Game& game, Player& player) {
    // TODO : implement ui later or something, decide later 
}

void UtilityTile::acquire(Game& game, Player& player) {
    if (this->getOwner() != nullptr) throw InvalidActionException("Utility already has an owner");
    if (this->getOwnershipStatus() != OwnershipStatus::BANK) throw InvalidActionException("Utility is not in BANK status");
    this->setOwner(&player);
    this->ownershipStatus = OwnershipStatus::OWNED;
    player.addProperty(this);
}

int UtilityTile::getBuildingValue() const {
    return 0;
}

int UtilityTile::getSelltoBankValue() const {
    return purchasePrice;
}