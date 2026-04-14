#include "model/managers/AuctionManager.hpp"

using namespace std;

AuctionManager::AuctionManager() {}

AuctionManager::AuctionManager(
    int currentBid,
    Player* highestBidder,
    const vector<Player*>& participants,
    int currentAuctionPlayerIndex,
    int consecutivePasses,
    bool auctionActive,
    bool hasAnyBid,
    shared_ptr<StreetTile> currentTile,
    Player* triggerPlayer
) {}

AuctionManager::AuctionManager(const AuctionManager& other) {}

AuctionManager::~AuctionManager() {}

AuctionManager& AuctionManager::operator=(const AuctionManager& other) {
    return *this;
}

void AuctionManager::initializeAuction(Game& game, StreetTile& tile, Player* triggerPlayer) {}

Player* AuctionManager::getCurrentAuctionPlayer() const {
    return nullptr;
}

bool AuctionManager::canBid(const Player& player, int amount) const {
    return false;
}

void AuctionManager::placeBid(Player& player, int amount) {}

bool AuctionManager::canPass() const {
    return false;
}

void AuctionManager::pass(Player& player) {}

bool AuctionManager::isAuctionOver() const {
    return false;
}

void AuctionManager::advanceToNextAuctionPlayer() {}

void AuctionManager::finalizeAuction(Game& game, StreetTile& tile) {}

bool AuctionManager::requiresForcedOpeningBid() const {
    return false;
}