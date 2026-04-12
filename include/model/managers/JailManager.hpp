#pragma once

#include <string>

using namespace std;

class Game;
class Player;

class JailManager {
public:
    JailManager();
    JailManager(const JailManager& other);
    ~JailManager();
    JailManager& operator=(const JailManager& other);

    void sendToJail(Game& game, Player& player);
    void payJailFine(Game& game, Player& player);
    bool tryRollForRelease(Game& game, Player& player);
    void releaseFromJail(Player& player);
    void beginJailDecision(Game& game, Player& player);
    void resolveJailDecision(Game& game, Player& player, const string& action);
};