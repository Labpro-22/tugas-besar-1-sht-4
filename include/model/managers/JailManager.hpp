#pragma once

#include <string>

using namespace std;

#include "model/Game.hpp"

class JailManager {
public:
    JailManager();
    JailManager(const JailManager& other);
    ~JailManager();
    JailManager& operator=(const JailManager& other);

    void sendToJail(Player& player);
    void payJailFine(Player& player, int amount);
    bool tryRollForRelease(Player& player);
    void releaseFromJail(Player& player);
    void beginJailDecision(Player& player);
    void resolveJailDecision(Player& player, const string& action);
};