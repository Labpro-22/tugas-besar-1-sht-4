#pragma once

#include <memory>
#include <vector>

#include "model/managers/CardDeck.hpp"

using namespace std;

class Game;
class Player;
class ChanceCard;
class CommunityChestCard;
class HandCard;

class CardManager {
private:
    CardDeck<ChanceCard> chanceDeck;
    CardDeck<CommunityChestCard> communityChestDeck;
    CardDeck<HandCard> handDeck;
    bool forceDropActive;

public:
    CardManager();
    CardManager(
        const CardDeck<ChanceCard>& chanceDeck,
        const CardDeck<CommunityChestCard>& communityChestDeck,
        const CardDeck<HandCard>& handDeck,
        bool forceDropActive
    );
    CardManager(const CardManager& other);
    ~CardManager();
    CardManager& operator=(const CardManager& other);

    void initializeDecks();
    void initializeChanceDeck();
    void initializeCommunityChestDeck();
    void initializeHandDeck();
    shared_ptr<ChanceCard> drawChanceCard();
    shared_ptr<CommunityChestCard> drawCommunityChestCard();
    shared_ptr<HandCard> drawHandCard();
    void discardHandCard(shared_ptr<HandCard> card);
    void reshuffleIfNeeded();
    void giveStartTurnCard(Player& player);
    void beginForceDrop(Game& game, Player& player);
    bool needsForceDrop(const Player& player) const;
    void useHandCard(Game& game, Player& player, int cardIndex);
    void updateTemporaryEffects(Player& player);
    void dropHandCard(Player& player, int cardIndex);
    vector<shared_ptr<HandCard>> getHandCards(const Player& player) const;

    CardDeck<HandCard>& getHandDeck();
    const CardDeck<HandCard>& getHandDeck() const;
};