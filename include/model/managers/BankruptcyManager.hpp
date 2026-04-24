#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

#include "model/GameContext.hpp"
#include "model/tiles/OwnableTile.hpp"

class BankruptcyManager {
public:
    struct DebtSession {
        Player* debtor;
        Player* creditor;
        int amount;
        bool debtToBank;
    };

private:
    int requiredAmount;
    Player* creditor;
    bool debtToBank;
    bool bankruptcyActive;
    Player* debtor;
    vector<DebtSession> sessionQueue;

public:
    BankruptcyManager();
    BankruptcyManager(
        int requiredAmount,
        Player* creditor,
        bool debtToBank,
        bool bankruptcyActive,
        Player* debtor
    );
    BankruptcyManager(const BankruptcyManager& other);
    ~BankruptcyManager();
    BankruptcyManager& operator=(const BankruptcyManager& other);

    bool canCoverDebt(const Player& player, int amount) const;
    int estimateLiquidationValue(const Player& player) const;
    void processDebtToPlayer(Player& debtor, Player& creditor, int amount);
    void processDebtToBank(Player& debtor, int amount);
    bool isDebtCovered(const Player& player) const;
    bool hasLiquidationOptions(const Player& player) const;
    bool isBankruptcyActive() const;
    int getPendingAmount() const;
    Player* getPendingCreditor() const;
    Player* getPendingDebtor() const;
    bool isPendingDebtToBank() const;
    void clearSession();
    void settleDebt(Player& debtor);
    void declareBankrupt(Player& debtor, Player* creditor);
    void beginBankruptcySession(Player& player, Player* creditor, int amount, bool debtToBank);
    void resolveLiquidationAction(Player& player, const string& action);
};