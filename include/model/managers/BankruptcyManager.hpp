#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class Game;
class Player;
class OwnableTile;

class BankruptcyManager {
private:
    int requiredAmount;
    shared_ptr<Player> creditor;
    bool debtToBank;
    bool bankruptcyActive;
    shared_ptr<Player> debtor;

public:
    BankruptcyManager();
    BankruptcyManager(
        int requiredAmount,
        shared_ptr<Player> creditor,
        bool debtToBank,
        bool bankruptcyActive,
        shared_ptr<Player> debtor
    );
    BankruptcyManager(const BankruptcyManager& other);
    ~BankruptcyManager();
    BankruptcyManager& operator=(const BankruptcyManager& other);

    bool canCoverDebt(const Player& player, int amount) const;
    int estimateLiquidationValue(const Player& player) const;
    void processDebtToPlayer(Game& game, Player& debtor, Player& creditor, int amount);
    void processDebtToBank(Game& game, Player& debtor, int amount);
    bool isDebtCovered(const Player& player) const;
    bool hasLiquidationOptions(const Game& game, const Player& player) const;
    bool isBankruptcyActive();
    void settleDebt(Game& game, Player& debtor);
    void declareBankrupt(Game& game, Player& debtor, shared_ptr<Player> creditor);
    void beginBankruptcySession(Game& game, Player& player, shared_ptr<Player> creditor, int amount, bool debtToBank);
    vector<shared_ptr<OwnableTile>> getSellableProperties(const Game& game, const Player& player) const;
    vector<shared_ptr<OwnableTile>> getMortgageableProperties(const Game& game, const Player& player) const;
    void resolveLiquidationAction(Game& game, Player& player, const string& action);
};