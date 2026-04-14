#pragma once

#include <memory>
#include <string>

#include "model/tiles/Tile.hpp"

using namespace std;

class Game;
class Player;

enum class OwnershipStatus {
    BANK,
    OWNED,
    MORTGAGED
};

class OwnableTile : public Tile {
protected:
    Player* owner;
    OwnershipStatus ownershipStatus;
    int purchasePrice;
    int mortgageValue;

public:
    OwnableTile();
    OwnableTile(
        int index,
        const string& code,
        const string& name,
        Player* owner,
        OwnershipStatus ownershipStatus,
        int purchasePrice,
        int mortgageValue
    );
    OwnableTile(const OwnableTile& other);
    virtual ~OwnableTile();
    OwnableTile& operator=(const OwnableTile& other);

    virtual void onLand(Game& game, Player& player) override = 0;
    Player* getOwner() const;
    void setOwner(Player* owner);
    bool isOwned() const;
    bool isMortgaged() const;
    void mortgage();
    void redeem();
    virtual int calculateRent(const Game& game, const Player& visitor) const = 0;
    virtual void acquire(Game& game, Player& player) = 0;

    OwnershipStatus getOwnershipStatus() const;
    int getPurchasePrice() const;
    int getMortgageValue() const;

    virtual int getBuildingValue() const = 0;
    virtual int getSelltoBankValue() const = 0;
};