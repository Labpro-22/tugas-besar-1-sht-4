#include "model/managers/TaxManager.hpp"

using namespace std;

//not used?
TaxManager::TaxManager() {}
//not used?
TaxManager::TaxManager(const TaxManager& other) {}
//not used?
TaxManager::~TaxManager() {}
//not used?
TaxManager& TaxManager::operator=(const TaxManager& other) {
    return *this;
}


int TaxManager::calculateTotalWealth(const Player& player) const {
    return player.getTotalWealth();
}

void TaxManager::processTaxPayment(Player& player, int taxAmount) {
    if (taxAmount > player.getMoney()) {
        throw ForcedInsufficientFundsException(taxAmount, player.getMoney());
    }
    player -= taxAmount;
}