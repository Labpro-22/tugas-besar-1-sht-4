#include "model/RentContext.hpp"

RentContext::RentContext()
    : ownerHasColorGroup(false),
      railroadRent(0),
      utilityMultiplier(0),
      diceTotal(0)
{}

RentContext::RentContext(
    bool ownerHasColorGroup,
    int railroadRent,
    int utilityMultiplier,
    int diceTotal
)
    : ownerHasColorGroup(ownerHasColorGroup),
      railroadRent(railroadRent),
      utilityMultiplier(utilityMultiplier),
      diceTotal(diceTotal)
{}

RentContext::RentContext(const RentContext& other)
    : ownerHasColorGroup(other.ownerHasColorGroup),
      railroadRent(other.railroadRent),
      utilityMultiplier(other.utilityMultiplier),
      diceTotal(other.diceTotal)
{}

RentContext::~RentContext() {}

RentContext& RentContext::operator=(const RentContext& other) {
    if (this != &other) {
        ownerHasColorGroup = other.ownerHasColorGroup;
        railroadRent = other.railroadRent;
        utilityMultiplier = other.utilityMultiplier;
        diceTotal = other.diceTotal;
    }
    return *this;
}

bool RentContext::getOwnerHasColorGroup() const {
    return ownerHasColorGroup;
}

int RentContext::getRailroadRent() const {
    return railroadRent;
}

int RentContext::getUtilityMultiplier() const {
    return utilityMultiplier;
}

int RentContext::getDiceTotal() const {
    return diceTotal;
}

void RentContext::setOwnerHasColorGroup(bool ownerHasColorGroup) {
    this->ownerHasColorGroup = ownerHasColorGroup;
}

void RentContext::setRailroadRent(int railroadRent) {
    this->railroadRent = railroadRent;
}

void RentContext::setUtilityMultiplier(int utilityMultiplier) {
    this->utilityMultiplier = utilityMultiplier;
}

void RentContext::setDiceTotal(int diceTotal) {
    this->diceTotal = diceTotal;
}
