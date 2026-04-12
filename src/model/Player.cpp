#include "model/Player.hpp"

using namespace std;

Player::Player() {}

Player::Player(
    const string& username,
    int money,
    int position,
    PlayerStatus status,
    const vector<shared_ptr<OwnableTile>>& ownedProperties,
    const vector<shared_ptr<HandCard>>& handCards,
    int failedJailRolls,
    bool usedHandCardThisTurn,
    bool shieldActive,
    int discountPercent,
    int discountDuration
) {}

Player::Player(const Player& other) {}

Player::~Player() {}

Player& Player::operator=(const Player& other) {
    return *this;
}

const string& Player::getUsername() const {
    return username;
}

int Player::getMoney() const {
    return money;
}

void Player::addMoney(int amount) {}

void Player::deductMoney(int amount) {}

void Player::moveTo(int position) {}

void Player::addProperty(shared_ptr<OwnableTile> property) {}

void Player::removeProperty(shared_ptr<OwnableTile> property) {}

void Player::addHandCard(shared_ptr<HandCard> card) {}

void Player::removeHandCard(int index) {}

bool Player::isBankrupt() const {
    return false;
}

bool Player::isJailed() const {
    return false;
}

void Player::setJailed(bool jailed) {}

int Player::getTotalWealth() const {
    return 0;
}

int Player::countProperties() const {
    return 0;
}

int Player::countCards() const {
    return 0;
}

Player& Player::operator+=(int amount) {
    return *this;
}

Player& Player::operator-=(int amount) {
    return *this;
}

Player Player::operator+(int amount) const {
    return *this;
}

Player Player::operator-(int amount) const {
    return *this;
}

bool Player::operator>(const Player& other) const {
    return false;
}

bool Player::operator<(const Player& other) const {
    return false;
}