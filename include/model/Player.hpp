#pragma once

#include <memory>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class HandCard;

enum class PlayerStatus {
    ACTIVE,
    JAILED,
    BANKRUPT
};

class Player {
private:
    string username;
    int money;
    int position;
    PlayerStatus status;
    vector<shared_ptr<HandCard>> handCards;
    int failedJailRolls;
    bool usedHandCardThisTurn;
    bool shieldActive;
    int discountPercent;
    int discountDuration;

public:
    Player();
    Player(
        const string& username,
        int money,
        int position,
        PlayerStatus status,
        int failedJailRolls,
        bool usedHandCardThisTurn,
        bool shieldActive,
        int discountPercent,
        int discountDuration
    );
    Player(const Player& other);
    ~Player();
    Player& operator=(const Player& other);

    const string& getUsername() const;
    int getMoney() const;
    int getPosition() const;
    void moveTo(int position);
    void addHandCard(shared_ptr<HandCard> card);
    void removeHandCard(int index);
    bool isBankrupt() const;
    bool isJailed() const;
    void setJailed(bool jailed);
    int countCards() const;

    Player& operator+=(int amount);
    Player& operator-=(int amount);
    Player operator+(int amount) const;
    Player operator-(int amount) const;
    bool operator>(const Player& other) const;
    bool operator<(const Player& other) const;
    vector<shared_ptr<HandCard>> getHandCards() const;
};
