#include "model/managers/SaveLoadManager.hpp"

using namespace std;

SaveLoadManager::SaveLoadManager() {}

SaveLoadManager::SaveLoadManager(const SaveLoadManager& other) {}

SaveLoadManager::~SaveLoadManager() {}

SaveLoadManager& SaveLoadManager::operator=(const SaveLoadManager& other) {
    return *this;
}

void SaveLoadManager::saveGame(const Game& game, const string& filename) {}

void SaveLoadManager::loadGame(Game& game, const string& filename) {}

void SaveLoadManager::savePlayers(const Game& game, ostream& out) {}

void SaveLoadManager::loadPlayers(Game& game, istream& in) {}

void SaveLoadManager::saveTurnState(const Game& game, ostream& out) {}

void SaveLoadManager::loadTurnState(Game& game, istream& in) {}

void SaveLoadManager::saveProperties(const Game& game, ostream& out) {}

void SaveLoadManager::loadProperties(Game& game, istream& in) {}

void SaveLoadManager::saveDecks(const Game& game, ostream& out) {}

void SaveLoadManager::loadDecks(Game& game, istream& in) {}

void SaveLoadManager::saveLogs(const Game& game, ostream& out) {}

void SaveLoadManager::loadLogs(Game& game, istream& in) {}