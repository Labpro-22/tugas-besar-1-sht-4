#include "model/cards/CommunityChestCard.hpp"

using namespace std;

CommunityChestCard::CommunityChestCard()
    : InstantCard() {}

CommunityChestCard::CommunityChestCard(const string& name, const string& description)
    : InstantCard(name, description) {}

CommunityChestCard::CommunityChestCard(const CommunityChestCard& other)
    : InstantCard(other) {}

CommunityChestCard::~CommunityChestCard() {}

CommunityChestCard& CommunityChestCard::operator=(const CommunityChestCard& other) {
    if (this != &other) {
        InstantCard::operator=(other);
    }
    return *this;
}