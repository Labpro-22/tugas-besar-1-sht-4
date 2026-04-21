#include "model/managers/CardManager.hpp"#include "model/cards/ChanceCard.hpp"
#include "model/cards/CommunityChestCard.hpp"
#include "model/cards/HandCard.hpp"
#include "model/cards/GoToNearestRailroadCard.hpp"
#include "model/cards/MoveBackThreeCard.hpp"
#include "model/cards/GoToJailCard.hpp"
#include "model/cards/BirthdayCard.hpp"
#include "model/cards/DoctorFeeCard.hpp"
#include "model/cards/CampaignFeeCard.hpp"
#include "model/cards/MoveCard.hpp"
#include "model/cards/DiscountCard.hpp"
#include "model/cards/ShieldCard.hpp"
#include "model/cards/TeleportCard.hpp"
#include "model/cards/LassoCard.hpp"
#include "model/cards/DemolitionCard.hpp"
#include "model/Game.hpp"
#include "model/Player.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

CardManager::CardManager() : forceDropActive(false) {}

CardManager::CardManager(
    const CardDeck<ChanceCard>& chanceDeck,
    const CardDeck<CommunityChestCard>& communityChestDeck,
    const CardDeck<HandCard>& handDeck,
    bool forceDropActive
) : chanceDeck(chanceDeck),
    communityChestDeck(communityChestDeck),
    handDeck(handDeck),
    forceDropActive(forceDropActive) {}

CardManager::CardManager(const CardManager& other)
    : chanceDeck(other.chanceDeck),
      communityChestDeck(other.communityChestDeck),
      handDeck(other.handDeck),
      forceDropActive(other.forceDropActive) {}

CardManager::~CardManager() {}

CardManager& CardManager::operator=(const CardManager& other) {
    if (this != &other) {
        chanceDeck = other.chanceDeck;
        communityChestDeck = other.communityChestDeck;
        handDeck = other.handDeck;
        forceDropActive = other.forceDropActive;
    }
    return *this;
}

void CardManager::initializeDecks() {
    initializeChanceDeck();
    initializeCommunityChestDeck();
    initializeHandDeck();
}

void CardManager::initializeChanceDeck() {
    chanceDeck.addCard(make_shared<GoToNearestRailroadCard>(
        "GoToNearestRailroad", "Pergi ke stasiun terdekat."
    ));
    chanceDeck.addCard(make_shared<MoveBackThreeCard>(
        "MoveBackThree", "Mundur 3 petak."
    ));
    chanceDeck.addCard(make_shared<GoToJailCard>(
        "GoToJail", "Masuk Penjara."
    ));
    chanceDeck.shuffle();
}

void CardManager::initializeCommunityChestDeck() {
    communityChestDeck.addCard(make_shared<BirthdayCard>(
        "Birthday", "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.", 100
    ));
    communityChestDeck.addCard(make_shared<DoctorFeeCard>(
        "DoctorFee", "Biaya dokter. Bayar M700.", 700
    ));
    communityChestDeck.addCard(make_shared<CampaignFeeCard>(
        "CampaignFee", "Anda mau nyaleg. Bayar M200 kepada setiap pemain.", 200
    ));
    communityChestDeck.shuffle();
}

void CardManager::initializeHandDeck() {
    for (int i = 0; i < 4; i++) {
        int steps = (rand() % 6) + 1;
        handDeck.addCard(make_shared<MoveCard>(
            "MoveCard", "Maju " + to_string(steps) + " Petak", false, steps
        ));
    }
    for (int i = 0; i < 3; i++) {
        int pct = (rand() % 5 + 1) * 10;         handDeck.addCard(make_shared<DiscountCard>(
            "DiscountCard", "Diskon " + to_string(pct) + "% untuk pembelian", false, pct, 1
        ));
    }
    for (int i = 0; i < 2; i++) {
        handDeck.addCard(make_shared<ShieldCard>(
            "ShieldCard", "Kebal tagihan atau sanksi selama 1 giliran", false, 1
        ));
    }
    for (int i = 0; i < 2; i++) {
        handDeck.addCard(make_shared<TeleportCard>(
            "TeleportCard", "Pindah ke petak manapun", false
        ));
    }
    for (int i = 0; i < 2; i++) {
        handDeck.addCard(make_shared<LassoCard>(
            "LassoCard", "Menarik satu lawan ke posisi Anda", false
        ));
    }
    for (int i = 0; i < 2; i++) {
        handDeck.addCard(make_shared<DemolitionCard>(
            "DemolitionCard", "Menghancurkan satu properti milik lawan", false
        ));
    }
    handDeck.shuffle();
}

shared_ptr<ChanceCard> CardManager::drawChanceCard() {
    return chanceDeck.drawCard();
}

shared_ptr<CommunityChestCard> CardManager::drawCommunityChestCard() {
    return communityChestDeck.drawCard();
}

shared_ptr<HandCard> CardManager::drawHandCard() {
    return handDeck.drawCard();
}

void CardManager::discardHandCard(shared_ptr<HandCard> card) {
    handDeck.discardCard(card);
}

void CardManager::reshuffleIfNeeded() {
    if (handDeck.isEmpty()) {
        handDeck.reshuffleDiscardIntoDrawPile();
    }
}

void CardManager::giveStartTurnCard(Player& player) {
    reshuffleIfNeeded();
    shared_ptr<HandCard> card = drawHandCard();
    if (!card) return;

    if (auto move = dynamic_pointer_cast<MoveCard>(card)) {
        int steps = (rand() % 6) + 1;
        card = make_shared<MoveCard>("MoveCard", "Maju " + to_string(steps) + " Petak", false, steps);
    } else if (auto disc = dynamic_pointer_cast<DiscountCard>(card)) {
        int pct = (rand() % 5 + 1) * 10;
        card = make_shared<DiscountCard>("DiscountCard", "Diskon " + to_string(pct) + "% untuk pembelian", false, pct, 1);
    }

    cout << "Kamu mendapatkan 1 kartu acak baru!" << endl;
    cout << "Kartu yang didapat: " << card->getName() << "." << endl;

    player.addHandCard(card);

    if (needsForceDrop(player)) {
        forceDropActive = true;
    }
}
bool CardManager::needsForceDrop(const Player& player) const {
    return player.countCards() > 3;
}

void CardManager::beginForceDrop(Game& game, Player& player) {
    auto hand = getHandCards(player);

    cout << "\nPERINGATAN: Kamu sudah memiliki 3 kartu di tangan (Maksimal 3)!"
         << " Kamu diwajibkan membuang 1 kartu.\n" << endl;

    cout << "Daftar Kartu Kemampuan Anda:" << endl;
    for (int i = 0; i < (int)hand.size(); i++) {
        cout << i + 1 << ". " << hand[i]->getName() << endl;
    }

    int choice = 0;
    while (choice < 1 || choice > (int)hand.size()) {
        cout << "\nPilih nomor kartu yang ingin dibuang (1-" << hand.size() << "): ";
        cin >> choice;
    }

    dropHandCard(player, choice - 1);
    forceDropActive = false;

    cout << hand[choice - 1]->getName() << " telah dibuang. "
         << "Sekarang kamu memiliki " << player.countCards() << " kartu di tangan." << endl;
}

void CardManager::useHandCard(Game& game, Player& player, int cardIndex) {
    auto hand = getHandCards(player);

    if (cardIndex < 0 || cardIndex >= (int)hand.size()) {
        cout << "Pilihan tidak valid." << endl;
        return;
    }

    shared_ptr<HandCard> card = hand[cardIndex];
    card->apply(game, player);

    discardHandCard(card);
    player.removeHandCard(cardIndex);
}


void CardManager::updateTemporaryEffects(Player& player) {
}

void CardManager::dropHandCard(Player& player, int cardIndex) {
    auto hand = getHandCards(player);
    if (cardIndex < 0 || cardIndex >= (int)hand.size()) return;

    discardHandCard(hand[cardIndex]);
    player.removeHandCard(cardIndex);
}

vector<shared_ptr<HandCard>> CardManager::getHandCards(const Player& player) const {
    return player.getHandCards();
}