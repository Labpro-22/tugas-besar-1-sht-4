#include "model/Player.hpp"

using namespace std;

Player::Player() {}

Player::Player(
        const string& username,
        int money,
        int position,
        PlayerStatus status,
        const vector<OwnableTile*>& ownedProperties,
        const vector<shared_ptr<HandCard>>& handCards,
        int failedJailRolls,
        bool usedHandCardThisTurn,
        bool shieldActive,
        int discountPercent,
        int discountDuration
    ) : username{username},
        money{money},
        position{position},
        status{status},
        ownedProperties{ownedProperties},
        handCards{handCards},
        failedJailRolls{failedJailRolls},
        usedHandCardThisTurn{usedHandCardThisTurn},
        shieldActive{shieldActive},
        discountPercent{discountPercent},
        discountDuration{discountDuration}
    {}

Player::Player(const Player& other) {
    this->username = other.username;
    this->money = other.money;
    this->position = other.position;
    this->status = other.status;
    this->ownedProperties = other.ownedProperties;
    this->handCards = other.handCards;
    this->failedJailRolls = other.failedJailRolls;
    this->usedHandCardThisTurn = other.usedHandCardThisTurn;
    this->shieldActive = other.shieldActive;
    this->discountPercent = other.discountPercent;
    this->discountDuration = other.discountDuration;
}

Player::~Player() {}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        this->username = other.username;
        this->money = other.money;
        this->position = other.position;
        this->status = other.status;
        this->ownedProperties = other.ownedProperties;
        this->handCards = other.handCards;
        this->failedJailRolls = other.failedJailRolls;
        this->usedHandCardThisTurn = other.usedHandCardThisTurn;
        this->shieldActive = other.shieldActive;
        this->discountPercent = other.discountPercent;
        this->discountDuration = other.discountDuration;
    }
    return *this;
}


const string& Player::getUsername() const {
    return username;
}

int Player::getMoney() const {
    return money;
}


void Player::moveTo(int position) {
    this->position = position;
}

void Player::addProperty(OwnableTile* property) {
    if (property != nullptr) {
        ownedProperties.push_back(property);
    }
}

void Player::removeProperty(OwnableTile* property) {
    auto it = find(ownedProperties.begin(), ownedProperties.end(), property);
    if (it != ownedProperties.end()) {
        ownedProperties.erase(it);
    }
}

void Player::addHandCard(shared_ptr<HandCard> card) {
    if (card != nullptr) {
        handCards.push_back(card);
    }
}

void Player::removeHandCard(int index) {
    if (index >= 0 && index < handCards.size()) {
        handCards.erase(handCards.begin() + index);
    }
}

bool Player::isBankrupt() const {
    if (status == PlayerStatus::BANKRUPT) {
        return true;
    }
    return false;
}

bool Player::isJailed() const {
    if (status == PlayerStatus::JAILED) {
        return true;
    }
    return false;
}

void Player::setJailed(bool jailed) {
    this->status = PlayerStatus::JAILED;
}

int Player::getTotalWealth() const {
    //TODO : hitungn total wealth dengan hitung money + harga tiap properti

    // int wealth = money;
    // for (const auto& property : ownedProperties) {
    //     wealth += property->purchasePrice;
    // }
    // return wealth;
}

int Player::countProperties() const {
    return ownedProperties.size();
}

int Player::countCards() const {
    return handCards.size();
}

Player& Player::operator+=(int amount) {
    this->money += amount;
    return *this;
}

Player& Player::operator-=(int amount) {
    this->money -= amount;
    return *this;
}

Player Player::operator+(int amount) const {
    Player result = *this;
    result.money += amount;
    return result;
}

Player Player::operator-(int amount) const {
    Player result = *this;
    result.money -= amount;
    return result;
}

bool Player::operator>(const Player& other) const {
    if (this->money != other.money) {
        return this->money > other.money;
    }
    return false;
}

bool Player::operator<(const Player& other) const {
    if (this->money != other.money) {
        return this->money < other.money;
    }
    return false;
}