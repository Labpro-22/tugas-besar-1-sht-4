#pragma once

#include <memory>
#include <string>

#include "model/tiles/Tile.hpp"

using namespace std;

class Game;
class Player;
class RentContext;

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

    virtual TileType onLand() const override = 0;
    Player* getOwner() const;
    void setOwner(Player* owner);
    void setOwnershipStatus(OwnershipStatus ownershipStatus);
    bool isOwned() const;
    bool isMortgaged() const;
    void mortgage();
    void redeem();
    virtual int calculateRent(const RentContext& rentContext) const = 0;
    virtual void acquire(Player& player) = 0;

    OwnershipStatus getOwnershipStatus() const;
    int getPurchasePrice() const;
    int getMortgageValue() const;

    virtual int getBuildingValue() const = 0;
    virtual int getSelltoBankValue() const = 0;
};