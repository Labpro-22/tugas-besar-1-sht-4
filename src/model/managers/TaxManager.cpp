#include "model/managers/TaxManager.hpp"

using namespace std;

TaxManager::TaxManager() {}

TaxManager::TaxManager(const TaxManager& other) {}

TaxManager::~TaxManager() {}

TaxManager& TaxManager::operator=(const TaxManager& other) {
    return *this;
}

int TaxManager::calculateTotalWealth(const Game& game, const Player& player) const {
    return 0;
}

void TaxManager::processTaxPayment(Game& game, Player& player, int taxAmount) {}