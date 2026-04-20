#include "model/managers/AuctionManager.hpp"

using namespace std;

AuctionManager::AuctionManager() {} // TODO: set default constructor

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
) : currentBid{currentBid},
    highestBidder{highestBidder},
    participants{participants},
    currentAuctionPlayerIndex{currentAuctionPlayerIndex},
    consecutivePasses{consecutivePasses},
    auctionActive{auctionActive},
    hasAnyBid{hasAnyBid},
    currentTile{currentTile},
    triggerPlayer{triggerPlayer}
    {}

AuctionManager::AuctionManager(const AuctionManager& other) {
    if (this != &other) {
        this->currentBid = other.currentBid;
        this->highestBidder = other.highestBidder;
        this->participants = other.participants;
        this->currentAuctionPlayerIndex = other.currentAuctionPlayerIndex;
        this->consecutivePasses = other.consecutivePasses;
        this->auctionActive = other.auctionActive;
        this->hasAnyBid = other.hasAnyBid;
        this->currentTile = other.currentTile;
        this->triggerPlayer = other.triggerPlayer;
    }
}

AuctionManager::~AuctionManager() {}

AuctionManager& AuctionManager::operator=(const AuctionManager& other) {
    if (this != &other) {
        this->currentBid = other.currentBid;
        this->highestBidder = other.highestBidder;
        this->participants = other.participants;
        this->currentAuctionPlayerIndex = other.currentAuctionPlayerIndex;
        this->consecutivePasses = other.consecutivePasses;
        this->auctionActive = other.auctionActive;
        this->hasAnyBid = other.hasAnyBid;
        this->currentTile = other.currentTile;
        this->triggerPlayer = other.triggerPlayer;
    }
    return *this;
}

void AuctionManager::initializeAuction(Game& game, StreetTile& tile, Player* triggerPlayer) {
    this->currentBid = 0;
    this->highestBidder = nullptr;
    this->participants.clear();
    for (Player& player : game.getPlayers()) {
        if (!player.isBankrupt()) {
            participants.push_back(&player);
        }
    }
    this->currentAuctionPlayerIndex = 0;
    this->consecutivePasses = 0;
    this->auctionActive = true;
    this->hasAnyBid = false;
    this->currentTile = make_shared<StreetTile>(tile);
    this->triggerPlayer = triggerPlayer;
}

Player* AuctionManager::getCurrentAuctionPlayer() const {
    return participants[currentAuctionPlayerIndex];
}

bool AuctionManager::canBid(const Player& player, int amount) const {
    if (player.getMoney() >= amount && amount > currentBid) return true;
    return false;
}

void AuctionManager::placeBid(Player& player, int amount) {
    if (canBid(player, amount)) {
        currentBid = amount;
        highestBidder = &player;
        hasAnyBid = true;
        consecutivePasses = 0;
    }
}

bool AuctionManager::canPass() const {
    return false;
}

void AuctionManager::pass(Player& player) {
    //Player buat apa?
    consecutivePasses++;
}

bool AuctionManager::isAuctionOver() const {
    return !auctionActive;
}

void AuctionManager::advanceToNextAuctionPlayer() {
    pass(*participants[currentAuctionPlayerIndex]);
    currentAuctionPlayerIndex = (currentAuctionPlayerIndex + 1) % participants.size();
}

void AuctionManager::finalizeAuction(Game& game, StreetTile& tile) {
    if (hasAnyBid && highestBidder != nullptr) {
        *highestBidder -= currentBid;
        tile.setOwner(highestBidder);
        auctionActive = false;
    }
}

bool AuctionManager::requiresForcedOpeningBid() const {
    return false;
}