#include "model/managers/BankruptcyManager.hpp"

using namespace std;

BankruptcyManager::BankruptcyManager()
    : requiredAmount(0),
      creditor(nullptr),
      debtToBank(true),
      bankruptcyActive(false),
      debtor(nullptr) {}

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

BankruptcyManager::~BankruptcyManager() {}

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
    return player.getMoney();
}

void BankruptcyManager::processDebtToPlayer(Player& debtor, Player& creditor, int amount) {
    if (debtor.getMoney() >= amount) {
        debtor -= amount;
        creditor += amount;
        return;
    }
    beginBankruptcySession(debtor, &creditor, amount, false);
}

void BankruptcyManager::processDebtToBank(Player& debtor, int amount) {
    if (debtor.getMoney() >= amount) {
        debtor -= amount;
        return;
    }
    beginBankruptcySession(debtor, nullptr, amount, true);
}

bool BankruptcyManager::isDebtCovered(const Player& player) const {
    return player.getMoney() >= requiredAmount;
}

bool BankruptcyManager::hasLiquidationOptions(const Player& player) const {
    return player.getMoney() > 0;
}

bool BankruptcyManager::isBankruptcyActive() const {
    return bankruptcyActive;
}

void BankruptcyManager::settleDebt(Player& debtor) {
    if (requiredAmount <= 0 || debtor.getMoney() < requiredAmount) {
        return;
    }

    debtor -= requiredAmount;
    if (!debtToBank && creditor != nullptr) {
        *creditor += requiredAmount;
    }
    bankruptcyActive = false;
    requiredAmount = 0;
    this->debtor = nullptr;
    this->creditor = nullptr;
}

void BankruptcyManager::declareBankrupt(Player& debtor, Player* creditor) {
    (void) creditor;
    debtor.setBankrupt(true);
    bankruptcyActive = true;
}

void BankruptcyManager::beginBankruptcySession(Player& player, Player* creditor, int amount, bool debtToBank) {
    this->debtor = &player;
    this->creditor = creditor;
    this->requiredAmount = amount;
    this->debtToBank = debtToBank;
    this->bankruptcyActive = true;
}

void BankruptcyManager::resolveLiquidationAction(Player& player, const string& action) {
    (void) action;
    if (isDebtCovered(player)) {
        settleDebt(player);
    }
}
