#pragma once

#include "model/tiles/OwnableTile.hpp"

using namespace std;

class Game;
class Player;

class UtilityTile : public OwnableTile {
public:
    UtilityTile();
    UtilityTile(
        int index,
        const string& code,
        const string& name,
        Player* owner,
        OwnershipStatus ownershipStatus,
        int purchasePrice,
        int mortgageValue
    );
    UtilityTile(const UtilityTile& other);
    ~UtilityTile();
    UtilityTile& operator=(const UtilityTile& other);

    int calculateRent(const Game& game, const Player& visitor) const override;
    void onLand(Game& game, Player& player) override;
    void acquire(Game& game, Player& player) override;

    int getBuildingValue() const override;
    int getSelltoBankValue() const override;
};
