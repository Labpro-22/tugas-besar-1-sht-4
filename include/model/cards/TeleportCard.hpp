#pragma once

#include "model/cards/HandCard.hpp"

using namespace std;

class Game;
class Player;

class TeleportCard : public HandCard {
public:
    TeleportCard();
    TeleportCard(const string& name, const string& description, bool used);
    TeleportCard(const TeleportCard& other);
    ~TeleportCard();
    TeleportCard& operator=(const TeleportCard& other);

    void setTargetTileIndex(int index);
    void apply(Game& game, Player& player) override;
private:
    int targetTileIndex;
};