#pragma once

using namespace std;

class Game;
class Player;

class TaxManager {
public:
    TaxManager();
    TaxManager(const TaxManager& other);
    ~TaxManager();
    TaxManager& operator=(const TaxManager& other);

    int calculateTotalWealth(const Game& game, const Player& player) const;
    void processTaxPayment(Game& game, Player& player, int taxAmount);
};