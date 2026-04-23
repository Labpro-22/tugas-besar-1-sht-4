#include "model/managers/JailManager.hpp"

#include "model/Dice.hpp"
#include "model/NimonException.hpp"
#include "model/Player.hpp"

using namespace std;

//Ga perlu
JailManager::JailManager() = default;

//Ga perlu
JailManager::JailManager(const JailManager& other) {}

//Ga perlu
JailManager::~JailManager() {}

//Ga perlu
JailManager& JailManager::operator=(const JailManager& other) {
    return *this;
}

void JailManager::sendToJail(Player& player) {
    player.setFailedJailRolls(0);
    player.setJailed(true);
}

void JailManager::payJailFine(Player& player, int amount) {
    if (player.getMoney() < amount){
        throw ForcedInsufficientFundsException(amount, player.getMoney());
    }
    player -= amount;
}

bool JailManager::tryRollForRelease(Dice& dice, Player& player) {
    dice.roll();
    if (dice.isDouble()) {
        releaseFromJail(player);
        return true;
    }
    return false;
}

void JailManager::releaseFromJail(Player& player) {
    player.setFailedJailRolls(0);
    player.setJailed(false);
}

void JailManager::beginJailDecision(Player& player) {}

void JailManager::resolveJailDecision(Player& player, const string& action) {}
