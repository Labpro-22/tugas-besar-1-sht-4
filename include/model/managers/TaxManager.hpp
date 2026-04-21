#pragma once

#include <vector>

using namespace std;

class OwnableTile;
class Player;

class TaxManager {
public:
    TaxManager();
    TaxManager(const TaxManager& other);
    ~TaxManager();
    TaxManager& operator=(const TaxManager& other);

    int calculateTotalWealth(int playerMoney, const vector<OwnableTile*>& ownedProperties) const;
    void processTaxPayment(Player& player, int taxAmount);
};