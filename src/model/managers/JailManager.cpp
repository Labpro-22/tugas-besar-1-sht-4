#include "model/managers/JailManager.hpp"

using namespace std;

JailManager::JailManager() {}

JailManager::JailManager(const JailManager& other) {}

JailManager::~JailManager() {}

JailManager& JailManager::operator=(const JailManager& other) {
    return *this;
}

void JailManager::sendToJail(Game& game, Player& player) {}

void JailManager::payJailFine(Game& game, Player& player) {}

bool JailManager::tryRollForRelease(Game& game, Player& player) {
    return false;
}

void JailManager::releaseFromJail(Player& player) {}

void JailManager::beginJailDecision(Game& game, Player& player) {}

void JailManager::resolveJailDecision(Game& game, Player& player, const string& action) {}