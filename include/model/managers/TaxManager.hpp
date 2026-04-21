#pragma once

using namespace std;

#include "model/GameContext.hpp"
#include "model/NimonException.hpp"

class TaxManager {
public:
    TaxManager();
    TaxManager(const TaxManager& other);
    ~TaxManager();
    TaxManager& operator=(const TaxManager& other);

    int calculateTotalWealth(const Player& player) const;
    void processTaxPayment(Player& player, int taxAmount);
};