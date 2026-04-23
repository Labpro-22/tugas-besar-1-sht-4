#include "model/managers/TaxManager.hpp"

#include "model/NimonException.hpp"
#include "model/Player.hpp"
#include "model/tiles/OwnableTile.hpp"

using namespace std;

//not used?
TaxManager::TaxManager() = default;
//not used?
TaxManager::TaxManager(const TaxManager& other) {}
//not used?
TaxManager::~TaxManager() {}
//not used?
TaxManager& TaxManager::operator=(const TaxManager& other) {
    return *this;
}


int TaxManager::calculateTotalWealth(int playerMoney, const vector<OwnableTile*>& ownedProperties) const {
    int totalWealth = playerMoney;
    for (OwnableTile* property : ownedProperties) {
        if (property != nullptr) {
            totalWealth += property->getPurchasePrice();
        }
    }
    return totalWealth;
}

void TaxManager::processTaxPayment(Player& player, int taxAmount) {
    if (taxAmount > player.getMoney()) {
        throw ForcedInsufficientFundsException(taxAmount, player.getMoney());
    }
    player -= taxAmount;
}