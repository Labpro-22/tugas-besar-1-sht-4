#pragma once

#include <istream>
#include <ostream>
#include <string>

using namespace std;

class Game;

class SaveLoadManager {
public:
    SaveLoadManager();
    SaveLoadManager(const SaveLoadManager& other);
    ~SaveLoadManager();
    SaveLoadManager& operator=(const SaveLoadManager& other);

    void saveGame(Game& game, const string& filename);
    void loadGame(Game& game, const string& filename);
    void savePlayers(const Game& game, ostream& out);
    void loadPlayers(Game& game, istream& in);
    void saveTurnState(const Game& game, ostream& out);
    void loadTurnState(Game& game, istream& in);
    void saveProperties(const Game& game, ostream& out);
    void loadProperties(Game& game, istream& in);
    void saveDecks(const Game& game, ostream& out);
    void loadDecks(Game& game, istream& in);
    void saveLogs(const Game& game, ostream& out);
    void loadLogs(Game& game, istream& in);
};
