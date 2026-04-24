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
        this->sessionQueue = other.sessionQueue;
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
        this->sessionQueue = other.sessionQueue;
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
    const int amount = sessionQueue.empty() ? requiredAmount : sessionQueue.front().amount;
    return player.getMoney() >= player.effectiveCost(amount);
}

bool BankruptcyManager::hasLiquidationOptions(const Player& player) const {
    return player.getMoney() > 0;
}

bool BankruptcyManager::isBankruptcyActive() const {
    return !sessionQueue.empty();
}

int BankruptcyManager::getPendingAmount() const {
    if (sessionQueue.empty()) return 0;
    return sessionQueue.front().amount;
}

Player* BankruptcyManager::getPendingCreditor() const {
    if (sessionQueue.empty()) return nullptr;
    return sessionQueue.front().creditor;
}

Player* BankruptcyManager::getPendingDebtor() const {
    if (sessionQueue.empty()) return nullptr;
    return sessionQueue.front().debtor;
}

bool BankruptcyManager::isPendingDebtToBank() const {
    if (sessionQueue.empty()) return true;
    return sessionQueue.front().debtToBank;
}

void BankruptcyManager::clearSession() {
    if (!sessionQueue.empty()) {
        sessionQueue.erase(sessionQueue.begin());
    }
    if (sessionQueue.empty()) {
        bankruptcyActive = false;
        requiredAmount = 0;
        creditor = nullptr;
        debtor = nullptr;
        debtToBank = true;
    } else {
        bankruptcyActive = true;
        requiredAmount = sessionQueue.front().amount;
        creditor = sessionQueue.front().creditor;
        debtor = sessionQueue.front().debtor;
        debtToBank = sessionQueue.front().debtToBank;
    }
}

void BankruptcyManager::settleDebt(Player& debtor) {
    if (sessionQueue.empty() && requiredAmount <= 0) {
        return;
    }

    const DebtSession session = sessionQueue.empty()
        ? DebtSession{&debtor, creditor, requiredAmount, debtToBank}
        : sessionQueue.front();
    if (session.amount <= 0 || debtor.getMoney() < debtor.effectiveCost(session.amount)) {
        return;
    }

    debtor -= session.amount;
    if (!session.debtToBank && session.creditor != nullptr) {
        *session.creditor += debtor.effectiveCost(session.amount);
    }
    if (!sessionQueue.empty()) {
        sessionQueue.erase(sessionQueue.begin());
    }
    bankruptcyActive = !sessionQueue.empty();
    requiredAmount = bankruptcyActive ? sessionQueue.front().amount : 0;
    this->debtor = bankruptcyActive ? sessionQueue.front().debtor : nullptr;
    this->creditor = bankruptcyActive ? sessionQueue.front().creditor : nullptr;
    this->debtToBank = bankruptcyActive ? sessionQueue.front().debtToBank : true;
}

void BankruptcyManager::declareBankrupt(Player& debtor, Player* creditor) {
    this->debtor = &debtor;
    this->creditor = creditor;
    debtor.setBankrupt(true);
    bankruptcyActive = true;
}

void BankruptcyManager::beginBankruptcySession(Player& player, Player* creditor, int amount, bool debtToBank) {
    sessionQueue.push_back({&player, creditor, amount, debtToBank});
    const DebtSession& activeSession = sessionQueue.front();
    this->debtor = activeSession.debtor;
    this->creditor = activeSession.creditor;
    this->requiredAmount = activeSession.amount;
    this->debtToBank = activeSession.debtToBank;
    this->bankruptcyActive = true;
}

void BankruptcyManager::resolveLiquidationAction(Player& player, const string& action) {
    (void) action;
    if (isDebtCovered(player)) {
        settleDebt(player);
    }
}
