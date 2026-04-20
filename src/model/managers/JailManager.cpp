#include "model/managers/JailManager.hpp"

using namespace std;

//Ga perlu
JailManager::JailManager() {}

//Ga perlu
JailManager::JailManager(const JailManager& other) {}

//Ga perlu
JailManager::~JailManager() {}

//Ga perlu
JailManager& JailManager::operator=(const JailManager& other) {
    return *this;
}

void JailManager::sendToJail(Game& game, Player& player) {
    player.setJailed(true);
}

void JailManager::payJailFine(Game& game, Player& player) {
    //TODO : check amount
    player -= 
}

bool JailManager::tryRollForRelease(Game& game, Player& player) {
    Dice& dice = game.getDice();
    dice.roll();
    if (dice.isDouble()) {
        releaseFromJail(player);
        return true;
    }
    return false;
}

void JailManager::releaseFromJail(Player& player) {
    player.setJailed(false);
}

void JailManager::beginJailDecision(Game& game, Player& player) {}

void JailManager::resolveJailDecision(Game& game, Player& player, const string& action) {}