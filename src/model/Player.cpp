#include "model/Player.hpp"

using namespace std;

Player::Player() {}

Player::Player(
        const string& username,
        int money,
        int position,
        PlayerStatus status,
        int failedJailRolls,
        bool usedHandCardThisTurn,
        bool shieldActive,
        int discountPercent,
        int discountDuration
    ) : username{username},
        money{money},
        position{position},
        status{status},
        // ownedProperties{ownedProperties},
        // handCards{handCards},
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

int Player::getPosition() const {
    return position;
}

PlayerStatus Player::getStatus() const {
    return status;
}

int Player::getFailedJailRolls() const {
    return failedJailRolls;
}

bool Player::isUsedHandCardThisTurn() const {
    return usedHandCardThisTurn;
}

bool Player::isShieldActive() const {
    return shieldActive;
}

int Player::getDiscountPercent() const {
    return discountPercent;
}

int Player::getDiscountDuration() const {
    return discountDuration;
}

void Player::setMoney(int money) {
    this->money = money;
}

void Player::setStatus(PlayerStatus status) {
    this->status = status;
}

void Player::setFailedJailRolls(int n) {
    this->failedJailRolls = n;
}

void Player::setUsedHandCardThisTurn(bool b) {
    this->usedHandCardThisTurn = b;
}

void Player::setShieldActive(bool b) {
    this->shieldActive = b;
}

void Player::setDiscount(int percent, int duration) {
    this->discountPercent = percent;
    this->discountDuration = duration;
    if (this->discountDuration <= 0) {
        this->discountDuration = 0;
        this->discountPercent = 0;
    }
}

void Player::decrementDiscountDuration() {
    if (discountDuration > 0) {
        discountDuration--;
    }
    if (discountDuration <= 0) {
        discountDuration = 0;
        discountPercent = 0;
    }
}

void Player::moveTo(int position) {
    this->position = position;
}

void Player::addHandCard(shared_ptr<HandCard> card) {
    if (card != nullptr) {
        handCards.push_back(card);
    }
}

void Player::removeHandCard(int index) {
    if (index >= 0 && index < static_cast<int>(handCards.size())) {
        handCards.erase(handCards.begin() + index);
    }
}

bool Player::isBankrupt() const {
    if (status == PlayerStatus::BANKRUPT) {
        return true;
    }
    return false;
}

void Player::setBankrupt(bool bankrupt) {
    if (bankrupt) {
        this->status = PlayerStatus::BANKRUPT;
    } else {
        this->status = PlayerStatus::ACTIVE;
    }
}

bool Player::isJailed() const {
    if (status == PlayerStatus::JAILED) {
        return true;
    }
    return false;
}

void Player::setJailed(bool jailed) {
    if (jailed) {
        this->status = PlayerStatus::JAILED;
    } else {
        this->status = PlayerStatus::ACTIVE;
    }
}

int Player::countCards() const {
    return static_cast<int>(handCards.size());
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

vector<shared_ptr<HandCard>> Player::getHandCards() const {
    return handCards;
}

bool Player::hasUsedHandCardThisTurn() const {
    return usedHandCardThisTurn;
}
