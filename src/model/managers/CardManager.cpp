#include "model/managers/CardManager.hpp"

using namespace std;

CardManager::CardManager() {}

CardManager::CardManager(
    const CardDeck<ChanceCard>& chanceDeck,
    const CardDeck<CommunityChestCard>& communityChestDeck,
    const CardDeck<HandCard>& handDeck,
    bool forceDropActive
) {}

CardManager::CardManager(const CardManager& other) {}

CardManager::~CardManager() {}

CardManager& CardManager::operator=(const CardManager& other) {
    return *this;
}

void CardManager::initializeDecks() {}

void CardManager::initializeChanceDeck() {}

void CardManager::initializeCommunityChestDeck() {}

void CardManager::initializeHandDeck() {}

shared_ptr<ChanceCard> CardManager::drawChanceCard() {
    return nullptr;
}

shared_ptr<CommunityChestCard> CardManager::drawCommunityChestCard() {
    return nullptr;
}

shared_ptr<HandCard> CardManager::drawHandCard() {
    return nullptr;
}

void CardManager::discardHandCard(shared_ptr<HandCard> card) {}

void CardManager::reshuffleIfNeeded() {}

void CardManager::giveStartTurnCard(Player& player) {}

void CardManager::beginForceDrop(Game& game, Player& player) {}

bool CardManager::needsForceDrop(const Player& player) const {
    return false;
}

void CardManager::useHandCard(Game& game, Player& player, int cardIndex) {}

void CardManager::updateTemporaryEffects(Player& player) {}

void CardManager::dropHandCard(Player& player, int cardIndex) {}

vector<shared_ptr<HandCard>> CardManager::getHandCards(const Player& player) const {
    return {};
}