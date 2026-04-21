#pragma once

#include <string>

using namespace std;

class Dice;
class Player;

class JailManager {
public:
    JailManager();
    JailManager(const JailManager& other);
    ~JailManager();
    JailManager& operator=(const JailManager& other);

    void sendToJail(Player& player);
    void payJailFine(Player& player, int amount);
    bool tryRollForRelease(Dice& dice, Player& player);
    void releaseFromJail(Player& player);
    void beginJailDecision(Player& player);
    void resolveJailDecision(Player& player, const string& action);
};