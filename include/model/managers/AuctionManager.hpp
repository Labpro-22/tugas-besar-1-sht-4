#pragma once

#include <memory>
#include <vector>

using namespace std;

#include "model/tiles/StreetTile.hpp"
#include "model/Player.hpp"
#include "model/Game.hpp"

class AuctionManager {
private:
    int currentBid;
    Player* highestBidder;
    vector<Player*> participants;
    int currentAuctionPlayerIndex;
    int consecutivePasses;
    bool auctionActive;
    bool hasAnyBid;
    shared_ptr<StreetTile> currentTile;
    Player* triggerPlayer;

public:
    AuctionManager();
    AuctionManager(
        int currentBid,
        Player* highestBidder,
        const vector<Player*>& participants,
        int currentAuctionPlayerIndex,
        int consecutivePasses,
        bool auctionActive,
        bool hasAnyBid,
        shared_ptr<StreetTile> currentTile,
        Player* triggerPlayer
    );
    AuctionManager(const AuctionManager& other);
    ~AuctionManager();
    AuctionManager& operator=(const AuctionManager& other);

    void initializeAuction(Game& game, StreetTile& tile, Player* triggerPlayer);
    Player* getCurrentAuctionPlayer() const;
    bool canBid(const Player& player, int amount) const;
    void placeBid(Player& player, int amount);
    bool canPass() const;
    void pass(Player& player);
    bool isAuctionOver() const;
    void advanceToNextAuctionPlayer();
    void finalizeAuction(Game& game, StreetTile& tile);
    bool requiresForcedOpeningBid() const;
};