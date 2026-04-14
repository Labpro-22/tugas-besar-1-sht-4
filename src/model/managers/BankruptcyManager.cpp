#include "model/managers/BankruptcyManager.hpp"

using namespace std;

BankruptcyManager::BankruptcyManager() {}

BankruptcyManager::BankruptcyManager(
    int requiredAmount,
    Player* creditor,
    bool debtToBank,
    bool bankruptcyActive,
    Player* debtor
) {}

BankruptcyManager::BankruptcyManager(const BankruptcyManager& other) {}

BankruptcyManager::~BankruptcyManager() {}

BankruptcyManager& BankruptcyManager::operator=(const BankruptcyManager& other) {
    return *this;
}

bool BankruptcyManager::canCoverDebt(const Player& player, int amount) const {
    return false;
}

int BankruptcyManager::estimateLiquidationValue(const Player& player) const {
    return 0;
}

void BankruptcyManager::processDebtToPlayer(Game& game, Player& debtor, Player& creditor, int amount) {}

void BankruptcyManager::processDebtToBank(Game& game, Player& debtor, int amount) {}

bool BankruptcyManager::isDebtCovered(const Player& player) const {
    return false;
}

bool BankruptcyManager::hasLiquidationOptions(const Game& game, const Player& player) const {
    return false;
}

bool BankruptcyManager::isBankruptcyActive() const {
    return bankruptcyActive;
}

void BankruptcyManager::settleDebt(Game& game, Player& debtor) {}

void BankruptcyManager::declareBankrupt(Game& game, Player& debtor, Player* creditor) {}

void BankruptcyManager::beginBankruptcySession(Game& game, Player& player, Player* creditor, int amount, bool debtToBank) {}

vector<shared_ptr<OwnableTile>> BankruptcyManager::getSellableProperties(const Game& game, const Player& player) const {
    return {};
}

vector<shared_ptr<OwnableTile>> BankruptcyManager::getMortgageableProperties(const Game& game, const Player& player) const {
    return {};
}

void BankruptcyManager::resolveLiquidationAction(Game& game, Player& player, const string& action) {}