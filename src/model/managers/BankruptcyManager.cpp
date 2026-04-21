#include "model/managers/BankruptcyManager.hpp"

using namespace std;

BankruptcyManager::BankruptcyManager() {}

BankruptcyManager::BankruptcyManager(
    int requiredAmount,
    Player* creditor,
    bool debtToBank,
    bool bankruptcyActive,
    Player* debtor
) :requiredAmount(requiredAmount), creditor(creditor), debtToBank(debtToBank), bankruptcyActive(bankruptcyActive), debtor(debtor) {}

BankruptcyManager::BankruptcyManager(const BankruptcyManager& other) {
    if (this != &other) {
        this->requiredAmount = other.requiredAmount;
        this->creditor = other.creditor;
        this->debtToBank = other.debtToBank;
        this->bankruptcyActive = other.bankruptcyActive;
        this->debtor = other.debtor;
    }
}

BankruptcyManager::~BankruptcyManager() {
    delete creditor;
    delete debtor;
}

BankruptcyManager& BankruptcyManager::operator=(const BankruptcyManager& other) {
    if (this != &other) {
        this->requiredAmount = other.requiredAmount;
        this->creditor = other.creditor;
        this->debtToBank = other.debtToBank;
        this->bankruptcyActive = other.bankruptcyActive;
        this->debtor = other.debtor;
    }
    return *this;
}

bool BankruptcyManager::canCoverDebt(const Player& player, int amount) const {
    if (player.getMoney() >= amount) {
        return true;
    }
    return false;
}

int BankruptcyManager::estimateLiquidationValue(const Player& player) const {
    return 0;
}

void BankruptcyManager::processDebtToPlayer(Player& debtor, Player& creditor, int amount) {}

void BankruptcyManager::processDebtToBank(Player& debtor, int amount) {}

bool BankruptcyManager::isDebtCovered(const Player& player) const {
    return false;
}

bool BankruptcyManager::hasLiquidationOptions(const Player& player) const {
    return false;
}

bool BankruptcyManager::isBankruptcyActive() const {
    return bankruptcyActive;
}

void BankruptcyManager::settleDebt(Player& debtor) {}

void BankruptcyManager::declareBankrupt(Player& debtor, Player* creditor) {}

void BankruptcyManager::beginBankruptcySession(Player& player, Player* creditor, int amount, bool debtToBank) {}

void BankruptcyManager::resolveLiquidationAction(Player& player, const string& action) {}