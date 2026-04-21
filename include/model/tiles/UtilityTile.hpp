#pragma once

#include "model/tiles/OwnableTile.hpp"

using namespace std;

class Game;
class Player;
class RentContext;

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

    int calculateRent(const RentContext& rentContext) const override;
    TileType onLand() const override;
    void acquire(Player& player) override;

    int getBuildingValue() const override;
    int getSelltoBankValue() const override;
};
