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
    PlayerStatus getStatus() const;
    int getFailedJailRolls() const;
    bool isUsedHandCardThisTurn() const;
    bool isShieldActive() const;
    int getDiscountPercent() const;
    int getDiscountDuration() const;

    void moveTo(int position);
    void setMoney(int money);
    void setStatus(PlayerStatus status);
    void setFailedJailRolls(int n);
    void setUsedHandCardThisTurn(bool b);
    void setShieldActive(bool b);
    void setDiscount(int percent, int duration);

    void addHandCard(shared_ptr<HandCard> card);
    void removeHandCard(int index);
    bool isBankrupt() const;
    void setBankrupt(bool bankrupt);
    bool isJailed() const;
    void setJailed(bool jailed);
    int countCards() const;

    Player& operator+=(int amount);
    Player& operator-=(int amount);
    Player operator+(int amount) const;
    Player operator-(int amount) const;
    int effectiveCost(int amount) const;
    bool operator>(const Player& other) const;
    bool operator<(const Player& other) const;
    vector<shared_ptr<HandCard>> getHandCards() const;

    void decrementDiscountDuration();

    bool hasUsedHandCardThisTurn() const;
};
