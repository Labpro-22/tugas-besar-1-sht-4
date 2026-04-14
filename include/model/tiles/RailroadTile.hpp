#pragma once

#include "model/tiles/OwnableTile.hpp"

using namespace std;

class Game;
class Player;

class RailroadTile : public OwnableTile {
public:
    RailroadTile();
    RailroadTile(
        int index,
        const string& code,
        const string& name,
        Player* owner,
        OwnershipStatus ownershipStatus,
        int purchasePrice,
        int mortgageValue
    );
    RailroadTile(const RailroadTile& other);
    ~RailroadTile();
    RailroadTile& operator=(const RailroadTile& other);

    int calculateRent(const Game& game, const Player& visitor) const override;
    void onLand(Game& game, Player& player) override;
    void acquire(Game& game, Player& player) override;

    int getBuildingValue() const override;
    int getSelltoBankValue() const override;
};